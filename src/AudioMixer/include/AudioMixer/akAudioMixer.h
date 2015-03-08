
#pragma once

#include <stdint.h>

class AudioMixerImpl;

namespace akAudioMixer
{
    class AudioSource;
    class MixControl;
    union MixEffect;
    enum class MixEffects: uint16_t;

    struct AudioMixerConfig
    {
        float mixAheadSeconds;
        uint32_t sampleFramesPerSecond;
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
        class MixHandle
        {
            friend class Impl;
        public:
            explicit MixHandle(uint32_t id = 0):
                m_id(id)
            {
            }

            explicit operator bool() const
            {
                return (m_id > 0);
            }

        protected:
            uint32_t m_id;
        };

        MixHandle AddSource(const AudioSource& source);
        MixControl GetMixControl(MixHandle handle);

        uint64_t GetCurTime() const;

        AudioBuffer Update(const uint32_t deltaNumSamples);
        AudioBuffer GetOutputBuffer();

        void PauseSource(MixHandle handle, bool resume = false);
        void AddEffect(MixHandle handle, const akAudioMixer::MixEffect& effect);
        void RemoveEffect(MixHandle handle, akAudioMixer::MixEffects type);

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
