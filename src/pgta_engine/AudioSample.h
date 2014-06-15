
#pragma once

#include <memory> 
#include <cstdint>

class AudioSample
{
public:
	AudioSample(AudioSample& other):
        m_samples(std::move(other.m_samples)),
        m_sampleRate(other.m_sampleRate),
        m_channels(other.m_channels),
        m_numSamples(other.m_numSamples),
        m_bitsPerSample(other.m_bitsPerSample)
	{
	}

    AudioSample(audio_data &data):
        m_samples(std::move(data.samples)),
        m_numSamples(data.numSamples),
        m_sampleRate(data.samplesPerSecond),
        m_bitsPerSample(data.bitsPerSample),
        m_channels(data.channels)
    {
    }
    
    char* getSamples() const
    {
        return m_samples.get();
    }
    uint32_t getSampleRate() const
    {
        return m_sampleRate;
    }
    uint16_t getBitsPerSample() const
    {
        return m_bitsPerSample;
    }
    uint16_t getChannels() const
    {
        return m_channels;
    }
    uint64_t getNumSamples() const
    {
        return m_numSamples;
    }
private:
    std::unique_ptr<char[]> m_samples;
    uint32_t m_sampleRate;
    uint16_t m_bitsPerSample;
    uint16_t m_channels;
    uint64_t m_numSamples;
};
