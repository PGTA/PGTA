
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

namespace akAudioMixer
{
    class AudioStream
    {
    public:
        AudioStream(uint32_t maxBufferedSamples = 44100U):
            m_samples(reinterpret_cast<int16_t*>(malloc(maxBufferedSamples*sizeof(int16_t)))),
            m_bufferSizeSamples(maxBufferedSamples)
        {
            assert(m_samples);
        }

        ~AudioStream()
        {
            free(m_samples);
        }

        uint32_t PushSamples(const int16_t* samples, uint32_t numSamples)
        {
            
        }

        uint32_t PopSamples(int16_t* samples, uint32_t numSamples)
        {

        }
        
    private:
        int16_t* m_samples;
        uint32_t m_bufferSizeSamples;
    };
}
