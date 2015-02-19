
#include <SDL.h>
#include <SDL_audio.h>
#include <akAudioMixer.h>

int mixerMain(const AudioSample& sample)
{
    akAudioMixer::AudioMixerConfig cfg;
    akAudioMixer::AudioMixer* mixer = akAudioMixer::CreateAudioMixer(cfg);
    if (!mixer)
    {
        return -1;
    }

    akAudioMixer::FreeAudioMixer(mixer);
    return 0;
}

int isPowerOfTwo(unsigned int x)
{
    return ((x > 0) && ((x & (~x + 1)) == x));
}

int main(int argc, const char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_AudioSpec spec{};
    Uint8* audioBuf = nullptr;
    Uint32 audioLen = 0;
    if (SDL_LoadWAV("media/thunder1.wav", &spec, &audioBuf, &audioLen) ||
        spec.format != AUDIO_S16 || !isPowerOfTwo(audioLen))
    {
        return -1;
    }

    AudioSample audioSample;
    //audioSample.samples = reinterpret_cast<const int16_t*>(audioBuf);
    //audioSample.numSamples = (audioLen >> 2);

    //int ret = mixerMain(samples, numSamples);

    SDL_FreeWAV(audioBuf);
    audioBuf = nullptr;
    audioLen = 0;

    SDL_Quit();
    return 0;
}
