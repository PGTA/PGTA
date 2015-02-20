
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include "akAudioStream.h"

namespace akAudioMixer
{
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

        uint32_t PopSamples(int16_t* samples, uint32_t numSamples)
        {
            if (m_stream)
            {
                return m_stream->PopSamples(samples, numSamples);
            }
            else if (m_samples && m_numSamples > 0)
            {
                const int16_t* bufSamples = m_samples;
                const uint32_t numBufSamples = m_numSamples;
                uint32_t curReadOffset = m_readOffset;
                const uint32_t samplesRemaining = numBufSamples - curReadOffset;
                numSamples = std::min(numSamples, samplesRemaining);

                const int16_t* readPtr = bufSamples + curReadOffset;
                std::copy(readPtr, readPtr + numSamples, samples);

                m_readOffset = curReadOffset + numSamples;
                return numSamples;
            }
            return 0;
        }

    private:
        AudioStream* m_stream;
        const int16_t* m_samples;
        uint32_t m_numSamples;
        uint32_t m_readOffset;
    };
}
