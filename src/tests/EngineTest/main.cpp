
#include <iostream>
#include <string.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <akPGTA.h>
#include <queue>

#include "FileUtils.h"
#include "utils.h"
#include "AudioBuffer.h"

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
    if (!ReadBinaryFileToString("tracks/rain_thunder.track", trackSource))
    {
        return -1;
    }

    HPGTATrack demoTrack = nullptr;
    const char* source = trackSource.data();
    const size_t length = trackSource.length();
    if (pgtaDevice.CreateTracks(1, &source, &length, &demoTrack) <= 0 || !demoTrack)
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
        const int samplesToWrite = len / sizeof(int16_t);
        const int samplesWritten = buffer->PopSamples(samples, samplesToWrite);

        // write silence for unfilled samples
        if (samplesWritten < 0)
        {
            printf("PopSamples failed!\n");
            memset(stream, 0, len);
        }
        else if (samplesWritten < samplesToWrite)
        {
            const int bytesWritten = samplesWritten * 2;
            memset(stream + bytesWritten, 0, len - bytesWritten);
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
