
#pragma once

#include "AudioData.h"
#include <chrono>
#include <memory> 
#include <cstdint>

class AudioSample
{
    using TimeDuration = std::chrono::high_resolution_clock::duration;
public:
	AudioSample(AudioSample& other):
        m_samples(std::move(other.m_samples)),
        m_duration(other.m_duration),
        m_numSamples(other.m_numSamples),
        m_sampleRate(other.m_sampleRate),
        m_bitsPerSample(other.m_bitsPerSample),
        m_channels(other.m_channels)
	{
	}

    AudioSample(audio_data &data):
        m_samples(std::move(data.samples)),
        m_numSamples(data.numSamples),
        m_sampleRate(data.samplesPerSecond),
        m_bitsPerSample(data.bitsPerSample),
        m_channels(data.channels)
    {
        using namespace std::chrono;
        double secs = static_cast<double>(data.numSamples) / data.samplesPerSecond;
        m_duration = duration_cast<TimeDuration>(duration<double>(secs));
    }
    
    const char* getSamples() const
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
    TimeDuration getDuration() const
    {
        return m_duration;
    }
private:
    std::unique_ptr<char[]> m_samples;
    TimeDuration m_duration;
    uint64_t m_numSamples;
    uint32_t m_sampleRate;
    uint16_t m_bitsPerSample;
    uint16_t m_channels;
};
