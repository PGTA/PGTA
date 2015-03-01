
#include <AudioMixer/akAudioSource.h>
#include <AudioMixer/akAudioStream.h>
#include <algorithm>

namespace akAudioMixer
{
    uint32_t AudioSource::PopSamples(int16_t* samples, uint32_t numSamples)
    {
        if (m_samples && (m_numSamples > 0) && samples && (numSamples > 0))
        {
            const int16_t* bufSamples = m_samples;
            const uint32_t numBufSamples = m_numSamples;
            const uint32_t curReadOffset = m_readOffset;
            const uint32_t samplesRemaining = numBufSamples - curReadOffset;
            numSamples = std::min(numSamples, samplesRemaining);

            const int16_t* readPtr = bufSamples + curReadOffset;
            std::copy(readPtr, readPtr + numSamples, samples);

            m_readOffset = curReadOffset + numSamples;
            return numSamples;
        }
        return 0;
    }

    uint32_t AudioSource::PopSamples(float* samples, uint32_t numSamples)
    {
        if (m_samples && (m_numSamples > 0) && samples && (numSamples > 0))
        {
            const int16_t* bufSamples = m_samples;
            const uint32_t numBufSamples = m_numSamples;
            const uint32_t curReadOffset = m_readOffset;
            const uint32_t samplesRemaining = numBufSamples - curReadOffset;
            numSamples = std::min(numSamples, samplesRemaining);

            const int16_t* readPtr = bufSamples + curReadOffset;
            const int16_t* readEnd = bufSamples + curReadOffset + numSamples;
            while (readPtr != readEnd)
            {
                const int16_t sample = *readPtr++;
                *samples++ = ((sample >= 0) ? (sample / 32767.0f) : (sample / 32768.0f));
            }

            m_readOffset = curReadOffset + numSamples;
            return numSamples;
        }
        return 0;
    }
}
