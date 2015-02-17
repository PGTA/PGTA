
#pragma once

#include <cstdint>
#include <queue>
#include <cstring>

class AudioBuffer
{
    struct AudioChunk
    {
        static const int MAX_SAMPLES = 1020;
        int64_t numSamples;
        int16_t samples[MAX_SAMPLES];
    };

public:
    AudioBuffer()
    {
        const int numBuffers = 64;
        AudioChunk* buffers = new AudioChunk[numBuffers];
        std::queue<AudioChunk*> freeList;
        for (int i = 0; i < numBuffers; ++i)
        {
            freeList.emplace(buffers + i);
        }

        m_freeBuffers = std::move(freeList);
        m_buffers = buffers;
    }

    ~AudioBuffer()
    {
        delete[] m_buffers;
    }

    int PushSamples(const int16_t* samples, int numSamples)
    {
        if (!samples || numSamples < 0)
        {
            printf("AudioBuffer::PushSamples: invalid args samples %p, numSamples %i!\n", samples, numSamples);
            return -1;
        }

        if (m_freeBuffers.empty())
        {
            printf("AudioBuffer::PushSamples: no free buffers!\n");
            return -1;
        }

        int numSamplesPushed = 0;
        while (numSamples > 0)
        {
            if (m_freeBuffers.empty())
            {
                printf("AudioBuffer::PushSamples: buffer overflow pushing %i samples!\n", numSamples);
                return numSamplesPushed;
            }

            AudioChunk* chunk = m_freeBuffers.front();
            m_freeBuffers.pop();

            const int numSamplesToCopy = (numSamples - AudioChunk::MAX_SAMPLES >= 0) ? AudioChunk::MAX_SAMPLES : numSamples;
            numSamples -= AudioChunk::MAX_SAMPLES;
            numSamplesPushed += numSamplesToCopy;

            chunk->numSamples = numSamplesToCopy;
            memcpy(chunk->samples, samples, numSamplesToCopy * sizeof(int16_t));
            samples += numSamplesToCopy;
            m_loadedBuffers.push(chunk);
        }
        return numSamplesPushed;
    }

    int PopSamples(int16_t* samples, int numSamples)
    {
        if (!samples || numSamples < 0)
        {
            printf("AudioBuffer::PopSamples: invalid args samples %p, numSamples %i!\n", samples, numSamples);
            return -1;
        }

        if (m_loadedBuffers.empty())
        {
            printf("AudioBuffer::PopSamples: no loaded buffers!\n");
            return -1;
        }

        int numSamplesPopped = 0;
        while (numSamples > 0)
        {
            if (m_loadedBuffers.empty())
            {
                printf("AudioBuffer::PopSamples: buffer underflow popping %i samples!\n", numSamples);
                return numSamplesPopped;
            }

            AudioChunk* chunk = m_loadedBuffers.front();
            const int numSamplesInChunk = static_cast<int>(chunk->numSamples);
            if (numSamplesInChunk <= numSamples)
            {
                memcpy(samples, chunk->samples, numSamplesInChunk * sizeof(int16_t));
                samples += numSamplesInChunk;
                numSamples -= numSamplesInChunk;
                numSamplesPopped += numSamplesInChunk;

                m_loadedBuffers.pop();
                m_freeBuffers.push(chunk);
            }
            else
            {
                numSamplesPopped += numSamples;
                memcpy(samples, chunk->samples, numSamples * sizeof(int16_t));
                memmove(chunk->samples, chunk->samples + numSamples, numSamples * sizeof(int16_t));
                chunk->numSamples = numSamples;
                break;
            }
        }
        return numSamplesPopped;
    }

private:
    std::queue<AudioChunk*> m_loadedBuffers;
    std::queue<AudioChunk*> m_freeBuffers;
    AudioChunk* m_buffers;
};
