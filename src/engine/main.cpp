
#include <iostream>
#include <string.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <akPGTA.h>
#include <queue>

#include "FileUtils.h"
#include "utils.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

void FixWorkingDirectory()
{
    char temp[128] = {};
    const char *dir = getcwd(temp, sizeof(temp));
    const char *bin_pos = strstr(dir, "bin");
    const char *build_pos = strstr(dir, "build");
    if (bin_pos)
    {
        chdir("..");
    }
    else if (build_pos)
    {
        chdir("../..");
    }
}

class AudioBuffer
{
    struct AudioChunk
    {
        static const int MAX_SAMPLES = 1020;
        int64_t numSamples;
        int16_t samples[MAX_SAMPLES];
    };

public:
    AudioBuffer()
    {
        const int numBuffers = 64;
        AudioChunk* buffers = new AudioChunk[numBuffers];
        std::queue<AudioChunk*> freeList;
        for (int i = 0; i < numBuffers; ++i)
        {
            freeList.emplace(buffers + i);
        }

        m_freeBuffers = std::move(freeList);
        m_buffers = buffers;
    }

    ~AudioBuffer()
    {
        delete[] m_buffers;
    }

    int PushSamples(const int16_t* samples, int numSamples)
    {
        if (!samples || numSamples < 0)
        {
            printf("AudioBuffer::PushSamples: invalid args samples %p, numSamples %i!\n", samples, numSamples);
            return -1;
        }

        int numSamplesPushed = 0;
        while (numSamples > 0)
        {
            if (m_freeBuffers.empty())
            {
                printf("AudioBuffer::PushSamples: buffer overflow pushing %i samples!\n", numSamples);
                return -1;
            }

            AudioChunk* chunk = m_freeBuffers.front();
            m_freeBuffers.pop();

            const int numSamplesToCopy = (numSamples - AudioChunk::MAX_SAMPLES >= 0) ? AudioChunk::MAX_SAMPLES : numSamples;
            numSamples -= AudioChunk::MAX_SAMPLES;
            numSamplesPushed += numSamplesToCopy;

            chunk->numSamples = numSamplesToCopy;
            memcpy(chunk->samples, samples, numSamplesToCopy * sizeof(int16_t));
            samples += numSamplesToCopy;
            m_loadedBuffers.push(chunk);
        }
        return numSamplesPushed;
    }

    int PopSamples(int16_t* samples, int numSamples)
    {
        if (!samples || numSamples < 0)
        {
            printf("AudioBuffer::PopSamples: invalid args samples %p, numSamples %i!\n", samples, numSamples);
            return -1;
        }

        int numSamplesPopped = 0;
        while (numSamples > 0)
        {
            if (m_loadedBuffers.empty())
            {
                printf("AudioBuffer::PopSamples: buffer underflow popping %i samples!\n", numSamples);
                return -1;
            }

            AudioChunk* chunk = m_loadedBuffers.front();
            const int numSamplesInChunk = static_cast<int>(chunk->numSamples);
            if (numSamplesInChunk <= numSamples)
            {
                memcpy(samples, chunk->samples, numSamplesInChunk * sizeof(int16_t));
                samples += numSamplesInChunk;
                numSamples -= numSamplesInChunk;
                numSamplesPopped += numSamplesInChunk;

                m_loadedBuffers.pop();
                m_freeBuffers.push(chunk);
            }
            else
            {
                numSamplesPopped += numSamples;
                memcpy(samples, chunk->samples, numSamples * sizeof(int16_t));
                memmove(chunk->samples, chunk->samples + numSamples, numSamples * sizeof(int16_t));
                chunk->numSamples = numSamples;
                break;
            }
        }
        return numSamplesPopped;
    }

private:
    std::queue<AudioChunk*> m_loadedBuffers;
    std::queue<AudioChunk*> m_freeBuffers;
    AudioChunk* m_buffers;
};

int pgtaMain(SDL_AudioDeviceID audioDevice, AudioBuffer* audioOut)
{
    PGTA::PGTADevice pgtaDevice;
    if (!pgtaDevice.Initialize())
    {
        return -1;
    }

    // load track data to memory
    // "tracks/demo.track"
    std::string trackSource;
    if (!ReadBinaryFileToString("tracks/demo.track", trackSource))
    {
        return -1;
    }

    HPGTATrack demoTrack = nullptr;
    const char* source = trackSource.data();
    if (pgtaDevice.CreateTracks(1, &source, &demoTrack) <= 0 || !demoTrack)
    {
        return -1;
    }

    PGTAConfig config;
    config.audioDesc.samplesPerSecond = 44100;
    config.audioDesc.bytesPerSample = 2;
    config.audioDesc.channels = 1;
    config.numBuffers = 4;
    config.bufferSizeInSamples = 8192;
    PGTA::PGTAContext pgtaContext(pgtaDevice.CreateContext(config));

    //pgtaContext.BindTrack(demoTrack);

    utils::RunLoop(10.0f, [&](double absoluteTime, float delta)
    {
        int32_t numBuffers = 0;
        const PGTABuffer* buffers = pgtaContext.Update(delta, &numBuffers);
        for (int32_t i = 0; i < numBuffers; ++i)
        {
            const PGTABuffer& buf = buffers[i];
            const int16_t* samples = reinterpret_cast<const int16_t*>(buf.samples);
            SDL_LockAudioDevice(audioDevice);
            if (audioOut->PushSamples(samples, buf.numSamples) < 0)
            {
                printf("PushSamples failed!\n");
            }
            SDL_UnlockAudioDevice(audioDevice);
        }

        return (buffers != nullptr);
    });

    return 0;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    FixWorkingDirectory();

    AudioBuffer audioBuffer;

    SDL_AudioSpec audioSpec;
    SDL_zero(audioSpec);
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 1;
    audioSpec.samples = 4096;
    audioSpec.callback = [](void *userdata, Uint8* stream, int len)
    {
        AudioBuffer* buffer = reinterpret_cast<AudioBuffer*>(userdata);
        int16_t* samples = reinterpret_cast<int16_t*>(stream);
        if (buffer->PopSamples(samples, len * sizeof(int16_t)) < 0)
        {
            printf("PopSamples failed!\n");
        }
    };
    audioSpec.userdata = &audioBuffer;
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, false, &audioSpec, nullptr, 0);

    int ret = 0;
    if (audioDevice > 0)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        ret = pgtaMain(audioDevice, &audioBuffer);
        SDL_CloseAudioDevice(audioDevice);
    }

    SDL_Quit();
    return ret;
}
