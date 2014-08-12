
#pragma once

#include "AudioSample.h"
#include <memory>

class EngineSample
{
    using TimeDuration = std::chrono::high_resolution_clock::duration;
public:
    struct SampleProps
    {
        // time difference from start of samples
        // to next potential playback 
        TimeDuration frequency;
        uint32_t probability;
        float volumeMultiplier;
	};

public:
    EngineSample(std::unique_ptr<AudioSample> sample, SampleProps &props):
        m_sample(std::move(sample)),
        m_props(props)
    {
    }

    EngineSample(EngineSample&& other):
        m_sample(std::move(other.m_sample)),
        m_props(other.m_props)
    {
    }

    const AudioSample* getSample() const
    {
        return m_sample.get();
    }
    
    TimeDuration GetFrequency() const
    {
        return m_props.frequency;
    }
    
    uint32_t GetProbability() const
    {
        return m_props.probability;
    }
    
    float GetVolumeMultiplier() const
    {
        return m_props.volumeMultiplier;
    }
    
private:
    std::unique_ptr<AudioSample> m_sample;
	SampleProps m_props;
    // TODO list of audio processors to apply
};

