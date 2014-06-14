
#pragma once

#include "DataBuffer.h"
#include <mutex>
#include <memory>

class AudioStreamBuffer
{
public:
    AudioStreamBuffer(int bitsPerSample, bool threadSafe = false):
        m_bytesPerSample(bitsPerSample / 8)
    {
        if (threadSafe)
        {
            m_bufferLock.reset(new std::mutex());
        }
    }

    void PushSamples(const char *samples, int numSamples)
    {
        auto numBytes = numSamples * m_bytesPerSample;
        LockBuffer();
        m_buffer.PushData(samples, numBytes);
        UnlockBuffer();
    }

    int PullSamples(char *samples, int numSamples)
    {
        auto numBytes = numSamples * m_bytesPerSample;
        LockBuffer();
        auto bytesGiven = m_buffer.PullData(samples, numBytes);
        UnlockBuffer();
        return bytesGiven / m_bytesPerSample;
    }

    int GetNumSamples()
    {   
        LockBuffer();
        int numSamples = m_buffer.GetNumBytes() / m_bytesPerSample;
        UnlockBuffer();
        return numSamples;
    }

    int GetBitsPerSample() const
    {
        return m_bytesPerSample * 8;
    }

private:
    void LockBuffer()
    {
        if (m_bufferLock)
        {
            m_bufferLock->lock();
        }
    }

    void UnlockBuffer()
    {
        if (m_bufferLock)
        {
            m_bufferLock->unlock();
        }
    }

private:
    DataBuffer<char> m_buffer;
    int m_bytesPerSample;
    std::unique_ptr<std::mutex> m_bufferLock;
};
