
#pragma once

#include <stdint.h>
#include "akAudioSample.h"

class AudioMixerImpl;

namespace akAudioMixer
{
    struct AudioMixerConfig;
    class MixControl;

    struct AudioBuffer
    {
        const int16_t* samples;
        uint64_t numSamples;
    };

    class AudioMixer
    {
    public:
        using Impl = AudioMixerImpl;
        struct MixHandle
        {
            uint64_t id;
        };

        MixHandle AddSample(AudioSample sample);
        MixControl* GetMixControl(MixHandle handle);

        AudioBuffer Update(const uint32_t deltaNumSamples);
        AudioBuffer GetOutputBuffer();

    protected:
        AudioMixer() = default;
        ~AudioMixer() = default;
        AudioMixer(const AudioMixer&) = delete;
        AudioMixer(const AudioMixer&&) = delete;
        AudioMixer& operator=(const AudioMixer&) = delete;
        AudioMixer& operator=(const AudioMixer&&) = delete;
    };

    AudioMixer* CreateAudioMixer(const AudioMixerConfig& cfg);
    void FreeAudioMixer(AudioMixer*& mixer);
}
