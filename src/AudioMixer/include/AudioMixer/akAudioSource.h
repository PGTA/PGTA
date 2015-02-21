
#pragma once

#include <stdint.h>

namespace akAudioMixer
{
    class AudioStream;
    class AudioSource
    {
    public:
        AudioSource():
            m_stream(nullptr),
            m_samples(nullptr),
            m_numSamples(0),
            m_readOffset(0)
        {
        }

        bool SetSource(AudioStream* stream)
        {
            if (!stream)
            {
                return false;
            }
            m_stream = stream;
            m_samples = nullptr;
            m_numSamples = 0;
            m_readOffset = 0;
            return true;
        }

        bool SetSource(const int16_t* samples, uint32_t numSamples)
        {
            if (!samples || numSamples == 0)
            {
                return false;
            }
            m_stream = nullptr;
            m_samples = samples;
            m_numSamples = numSamples;
            m_readOffset = 0;
            return true;
        }

        uint32_t PopSamples(int16_t* samples, uint32_t numSamples);

    private:
        AudioStream* m_stream;
        const int16_t* m_samples;
        uint32_t m_numSamples;
        uint32_t m_readOffset;
    };
}
