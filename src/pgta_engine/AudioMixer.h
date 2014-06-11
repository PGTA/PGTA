
#pragma once

#include <algorithm>

class AudioMixer
{
public:
    void addInput( EngineSampleBuffer *sampleBuffer )
    {
        m_inputSamples.push_back(sampleBuffer);
    }
    void mix( float delta )
    {
        float deltaSec = delta / 1000.0f; 
        // HACKHACKHACK 44100Hz  
        uint32_t numSamples = 44100 * deltaSec;
        m_outputBuffer.resize(numSamples);
        std::fill(m_outputBuffer.begin(), m_outputBuffer.end(), 0);
        for (auto &sampleBuffer : m_inputSampleBuffers)
        {
            int16_t *samples = nullptr; 
            auto samplesGiven = sampleBuffer.getAudioSamples(&samples, numSamples);
            combineSamples(samples, samplesGiven);
        }
    }
    
    void combineSamples( int16_t *samples, uint32_t numSamples )
    {
        auto minSampleCount = std::min(numSamples, m_outputBuffer.size());
        for ( uint32_t i = 0; i < minSampleCount; ++i )
        {
            m_outputBuffer[i] += samples[i];
        }
    }

private:
    std::vector<EngineSampleBuffer*> m_inputSampleBuffers;
    std::vector<int16_t> m_outputBuffer;
};

