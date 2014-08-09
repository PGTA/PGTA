
#pragma once

#include "AudioStreamBuffer.h"
#include "SDL_audio.h"
#include <cassert>
#include <vector>

class AudioSteamBuffer;

template<int BITS_PER_SAMPLE>
class AudioMixer
{
public:
    AudioMixer():
        m_outputStream(nullptr)
    {
    }
    void Mix()
    {
        auto bytesPerSample = BITS_PER_SAMPLE / 8;
        int minByteCount = std::numeric_limits<int>::max();
        for ( auto *inputStream : m_inputStreams )
        {
            auto numBytes = inputStream->GetNumSamples() * bytesPerSample;
            if (numBytes == 0)
                continue;
            minByteCount = std::min(minByteCount, numBytes);
        }
        
        if (minByteCount == std::numeric_limits<int>::max())
            return;
        
        m_mixBuffer.resize(minByteCount, 0);
        m_inputBuffer.resize(minByteCount, 0);
        std::fill(m_mixBuffer.begin(), m_mixBuffer.end(), 0);

        for ( auto *inputStream : m_inputStreams )
        {
            int numSamples = inputStream->PullSamples(m_inputBuffer.data(), minByteCount / bytesPerSample);
            
            // HACKHACKHACK - AUDIO_S16 need to be determined for all BPS values
            SDL_MixAudioFormat( (uint8_t*)m_mixBuffer.data(), (uint8_t*)m_inputBuffer.data(), AUDIO_S16, 
                numSamples*bytesPerSample, 128);

            // for ( int i = 0; i < numSamples; ++i )
            // {
            //     *mixBuff = (*mixBuff + *inputBuff++) / 2;
            //     ++mixBuff;
            // }
        }
        m_outputStream->PushSamples(m_mixBuffer.data(), m_mixBuffer.size() / bytesPerSample);
        
    }
    void ConnectInputStream( AudioStreamBuffer* inputStream )
    {
        assert(BITS_PER_SAMPLE == inputStream->GetBitsPerSample());
        m_inputStreams.push_back(inputStream);
    }
    
    void ConnectOutputStream( AudioStreamBuffer* outputStream )
    {
        assert(BITS_PER_SAMPLE == outputStream->GetBitsPerSample());
        m_outputStream = outputStream;
    }
private:
    std::vector<char> m_mixBuffer;
    std::vector<char> m_inputBuffer;
    std::vector<AudioStreamBuffer*> m_inputStreams;
    AudioStreamBuffer *m_outputStream;
};
