
#include <SDL.h>
#include <SDL_audio.h>
#include <akAudioMixer.h>
#include <assert.h>
#include <signal.h>
#include <iostream>
#include "utils.h"

class SDLWav
{
public:
    SDLWav(const char* filename):
        m_audioBuf(nullptr),
        m_audioLen(0),
        m_spec()
    {
        SDL_LoadWAV(filename, &m_spec, &m_audioBuf, &m_audioLen);
        assert(m_spec.freq == 44100);
        assert(m_spec.format == AUDIO_S16);
        assert(m_spec.channels == 1);
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

int mixerMain(SDL_AudioDeviceID audioDevice, const SDLWav& wav)
{
    akAudioMixer::AudioMixerConfig cfg;
    cfg.mixAheadSeconds = 1.0f;
    akAudioMixer::AudioMixer* mixer = akAudioMixer::CreateAudioMixer(cfg);
    if (!mixer)
    {
        return -1;
    }

    akAudioMixer::AudioSource source;
    source.SetSource(wav.GetSamplePtr(), wav.GetNumSamples());
    mixer->AddSource(source);

    utils::RunLoop(0.01f, [&](double absoluteTime, float delta)
    {
        const uint32_t deltaSamples = static_cast<uint32_t>(round(delta * 44100.0f));
        akAudioMixer::AudioBuffer output = mixer->Update(deltaSamples);
        SDL_QueueAudio(audioDevice, output.samples, output.numSamples*2);
        return (output.samples != nullptr) && (output.samples > 0);
    });

    akAudioMixer::FreeAudioMixer(mixer);
    return 0;
}

static void quit(int param)
{
    exit(1);
}

int main(int argc, const char* argv[])
{
    std::cout.sync_with_stdio(false);
    signal(SIGINT, &quit);
    utils::FixWorkingDirectory();
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_AudioSpec audioSpec;
    SDL_zero(audioSpec);
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = 1;
    audioSpec.samples = 4096;
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, false, &audioSpec, nullptr, 0);

    std::cout << "Drivers:\n";
    int numDrivers = SDL_GetNumAudioDrivers();
    for (int i = 0; i < numDrivers; ++i)
    {
        std::cout << SDL_GetAudioDriver(i) << "\n";
    }
    std::cout << "Devices:\n";
    int numDevices = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < numDevices; ++i)
    {
        std::cout << SDL_GetAudioDeviceName(i, 0) << "\n";
    }

    SDLWav thunder("media/thunder2.wav");

    int ret = -1;
    if (audioDevice > 0 && thunder)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        ret = mixerMain(audioDevice, thunder);
        SDL_CloseAudioDevice(audioDevice);
    }

    SDL_Quit();
    return ret;
}
