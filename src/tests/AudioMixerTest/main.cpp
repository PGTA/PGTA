
#include <SDL.h>
#include <SDL_audio.h>
#include <akAudioMixer.h>
#include <assert.h>
#include "utils.h"

int isPowerOfTwo(unsigned int x)
{
    return ((x > 0) && ((x & (~x + 1)) == x));
}

class SDLWav
{
public:
    SDLWav(const char* filename):
        m_audioBuf(nullptr),
        m_audioLen(0),
        m_spec()
    {
        SDL_LoadWAV(filename, &m_spec, &m_audioBuf, &m_audioLen);
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
        assert(m_spec.format == AUDIO_S16);
        return m_spec;
    }

    const int16_t* GetSamplePtr() const
    {
        return reinterpret_cast<const int16_t*>(m_audioBuf);
    }

    uint32_t GetNumSamples() const
    {
        assert(isPowerOfTwo(m_audioLen));
        return (m_audioLen >> 2);
    }

private:
    Uint8* m_audioBuf;
    Uint32 m_audioLen;
    SDL_AudioSpec m_spec;
};

int mixerMain(const SDLWav& wav)
{
    akAudioMixer::AudioMixerConfig cfg;
    cfg.mixAheadSeconds = 0.01f;
    akAudioMixer::AudioMixer* mixer = akAudioMixer::CreateAudioMixer(cfg);
    if (!mixer)
    {
        return -1;
    }

    akAudioMixer::AudioSource source;
    source.SetSource(wav.GetSamplePtr(), wav.GetNumSamples());
    mixer->AddSource(&source);

    utils::RunLoop(10.0f, [&](double absoluteTime, float delta)
    {
        const uint32_t deltaSamples = delta * 100.0f * 44100;
        akAudioMixer::AudioBuffer output = mixer->Update(deltaSamples);
        return (output.samples != nullptr);
    });

    akAudioMixer::FreeAudioMixer(mixer);
    return 0;
}

int main(int argc, const char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDLWav thunder("media/thunder1.wav");
    if (!thunder)
    {
        return -1;
    }

    int ret = mixerMain(thunder);

    SDL_Quit();
    return ret;
}
