
#pragma once

#include <memory> 
#include <cstdint>

class AudioSample
{
    
public:
	AudioSample(AudioSample& other)
	{
		auto ptr = other.m_samples.get();
		other.m_samples.release();
		m_samples.reset(ptr);
		m_sampleRate = other.m_sampleRate;
		m_channels = other.m_channels;
		m_numSamples = other.m_numSamples;
		m_bitsPerSample = other.m_bitsPerSample;
	}
    AudioSample( char *samples, uint32_t numSamples, uint32_t sampleRate, uint16_t bitsPerSample, uint16_t channels ) :
        m_samples(samples),
        m_numSamples(numSamples),
        m_sampleRate(sampleRate),
        m_bitsPerSample(bitsPerSample),
        m_channels(channels)
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
    uint32_t getNumSamples() const
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
