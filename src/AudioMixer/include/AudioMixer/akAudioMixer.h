
#pragma once

#include <stdint.h>

class AudioMixerImpl;

namespace akAudioMixer
{
    class AudioSource;
    class MixControl;

    struct AudioMixerConfig
    {
        float mixAheadSeconds;
    };

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
            explicit operator bool() const
            {
                return (id > 0);
            }
            uint64_t id;
        };

        MixHandle AddSource(AudioSource* source);
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
