
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
    // mix ahead time needs to be dt at minimum,
    // but larger in most cases to catch misbehaving timers
    // which can jump for 15-30 ms periodically
    cfg.mixAheadSeconds = 0.1f;
    cfg.sampleFramesPerSecond = 44100;
    akAudioMixer::AudioMixer* mixer = akAudioMixer::CreateAudioMixer(cfg);
    if (!mixer)
    {
        return -1;
    }

    /*{
        akAudioMixer::AudioSource source;
        source.SetSource(wav.GetSamplePtr(), wav.GetNumSamples());
        auto handle = mixer->AddSource(source);
        akAudioMixer::MixControl mixControl = mixer->GetMixControl(handle);
        if (mixControl)
        {
            akAudioMixer::MixEffect effect;
            effect.type = akAudioMixer::MixEffects::MixEffect_Gain;
            effect.gain.dBGain = 18.0f;
            mixControl.AddEffect(effect);
            //mixControl.RemoveEffect(akAudioMixer::MixEffects::MixEffect_Gain);
        }
    }*/

    SDLWav audio1("media/loon.wav");
    SDLWav audio2("media/frogs.wav");
    SDLWav audio3("media/parakeet.wav");
    {
        akAudioMixer::AudioSource source;
        source.SetSource(audio1.GetSamplePtr(), audio1.GetNumSamples());
        mixer->AddSource(source);
    }
    {
        akAudioMixer::AudioSource source;
        source.SetSource(audio2.GetSamplePtr(), audio2.GetNumSamples());
        akAudioMixer::MixEffect effect;
        effect.type = akAudioMixer::MixEffects::MixEffect_Gain;
        effect.gain.dBGain = 6.0f;
        mixer->AddEffect(mixer->AddSource(source), effect);
    }
    {
        akAudioMixer::AudioSource source;
        source.SetSource(audio3.GetSamplePtr(), audio3.GetNumSamples());
        mixer->AddSource(source);
    }

    const uint32_t rainSampleLength = wav.GetNumSamples();
    uint32_t rainNextScheduleCountdown = 0;

    akAudioMixer::MixEffect rainGain;
    rainGain.type = akAudioMixer::MixEffects::MixEffect_Gain;
    rainGain.gain.dBGain = 12.0f;

    akAudioMixer::MixEffect rainFade;
    rainFade.type = akAudioMixer::MixEffects::MixEffect_Fade;
    rainFade.fade.fadeOverNumSamples = rainSampleLength >> 2;
    rainFade.fade.dBInitial = -48.0f;
    rainFade.fade.dBFinal = 0.0f;

    utils::RunLoop(0.01f, [&](double /*absoluteTime*/, float delta)
    {
        const uint32_t deltaSamples = static_cast<uint32_t>(round(delta * 44100.0f));
        if (rainNextScheduleCountdown >= deltaSamples)
        {
            akAudioMixer::AudioBuffer output = mixer->Update(deltaSamples);
            SDL_QueueAudio(audioDevice, output.samples, static_cast<Uint32>(output.numSamples*2));
            rainNextScheduleCountdown -= deltaSamples;
        }
        else
        {
            const uint32_t leftoverTime = deltaSamples - rainNextScheduleCountdown;
            {
                akAudioMixer::AudioBuffer output = mixer->Update(rainNextScheduleCountdown);
                SDL_QueueAudio(audioDevice, output.samples, static_cast<Uint32>(output.numSamples*2));
            }
            rainNextScheduleCountdown = rainSampleLength - leftoverTime;
            {
                akAudioMixer::AudioSource source;
                source.SetSource(wav.GetSamplePtr(), wav.GetNumSamples());
                auto handle = mixer->AddSource(source);
                assert(handle);
                mixer->AddEffect(handle, rainGain);
                mixer->AddEffect(handle, rainFade);
            }
            {
                akAudioMixer::AudioBuffer output = mixer->Update(leftoverTime);
                SDL_QueueAudio(audioDevice, output.samples, static_cast<Uint32>(output.numSamples*2));
            }
        }
        return true;
    });

    akAudioMixer::FreeAudioMixer(mixer);
    return 0;
}

static void quit(int /*param*/)
{
    exit(1);
}

int main()
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

    SDLWav thunder("media/rainloop.wav");

    int ret = 0;
    while (audioDevice > 1 && thunder && !ret)
    {
        SDL_PauseAudioDevice(audioDevice, 0);
        ret = mixerMain(audioDevice, thunder);
        SDL_PauseAudioDevice(audioDevice, 1);
    }

    SDL_CloseAudioDevice(audioDevice);
    SDL_Quit();
    return ret;
}
