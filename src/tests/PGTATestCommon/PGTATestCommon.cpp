
#include "PGTATestCommon.h"


#include <SDL.h>
#include <SDL_audio.h>
#include <akAudioMixer.h>
#include <assert.h>
#include <signal.h>
#include <iostream>
#include <akPGTA.h>
#include "utils.h"
#include "FileUtils.h"

class SDLWavException : public std::exception
{
public:
    SDLWavException(std::string message) :
        m_message(message)
    {
    }
    virtual const char* what() const throw()
    {
        return m_message.c_str();
    }
private:
    std::string m_message;
};

class SDLWav
{
public:
    SDLWav(const char* filename) :
        m_audioBuf(nullptr),
        m_audioLen(0),
        m_spec()
    {
        SDL_LoadWAV(filename, &m_spec, &m_audioBuf, &m_audioLen);
        if (m_spec.freq != 44100 || m_spec.format != AUDIO_S16 || m_spec.channels != 1)
        {
            throw SDLWavException("Error loading file "+std::string(filename));
        }
        std::cout << filename << " has " << (m_audioLen >> 1) << " samples\n";
    }

    ~SDLWav()
    {
        if (m_audioBuf)
        {
            SDL_FreeWAV(m_audioBuf);
        }
    }

    explicit operator bool() const
    {
        return (m_audioBuf != nullptr && m_audioLen > 0);
    }

    const SDL_AudioSpec& GetSpec() const
    {
        return m_spec;
    }

    const int16_t* GetSamplePtr() const
    {
        return reinterpret_cast<const int16_t*>(m_audioBuf);
    }

    uint32_t GetNumSamples() const
    {
        assert((m_audioLen % 2) == 0);
        return (m_audioLen >> 1);
    }

private:
    Uint8* m_audioBuf;
    Uint32 m_audioLen;
    SDL_AudioSpec m_spec;
};

int pgtaMain(SDL_AudioDeviceID audioDevice, const std::string &trackName, std::atomic<int> &playbackControl)
{
    PGTA::PGTADevice pgtaDevice;
    if (!pgtaDevice.Initialize())
    {
        return -1;
    }

    // load track data to memory
    std::string trackSource;
    if (!ReadBinaryFileToString(trackName, trackSource))
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

    auto data = pgtaGetTrackData(demoTrack);
    int numSamples = data.numSamples;

    std::vector<SDLWav> audioFiles;
    audioFiles.reserve(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        int16_t id = data.samples[i].id;
        const char* file = data.samples[i].defaultFile;
        audioFiles.emplace_back(file);
        pgtaBindTrackSample(demoTrack, id, audioFiles[i].GetSamplePtr(), audioFiles[i].GetNumSamples());
    }

    PGTAConfig config;
    config.audioDesc.samplesPerSecond = 44100;
    config.audioDesc.bytesPerSample = 2;
    config.audioDesc.channels = 1;
    config.beatsPerMinute = 120;
    config.mixAhead = 0.5f;
    PGTA::PGTAContext pgtaContext(pgtaDevice.CreateContext(config));

    pgtaContext.BindTrack(demoTrack);
    utils::RunLoop(0.01f, [&](double /*absoluteTime*/, float delta)
    {
        const PGTABuffer output = pgtaContext.Update(delta);
        SDL_QueueAudio(audioDevice, output.samples, static_cast<Uint32>(output.numSamples * 2));
        return (output.samples != nullptr) && (output.numSamples > 0) && playbackControl.load() == 0;
    });
    return 0;
}

void PGTATestCommon::PlayTrack(std::string trackName, std::atomic<int> &playbackControl)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_AudioSpec audioSpec;
    SDL_zero(audioSpec);
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 1;
    audioSpec.samples = 4096;
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, false, &audioSpec, nullptr, 0);

    int ret = 0;
    if (audioDevice > 1)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        ret = pgtaMain(audioDevice, trackName, playbackControl);
        SDL_PauseAudioDevice(audioDevice, 1);
        SDL_CloseAudioDevice(audioDevice);
    }
    (void)ret;

    SDL_Quit();
}
