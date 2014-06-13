
#pragma once

#include "EngineSample.h"
#include "EngineGroup.h"
#include <vector>

class EngineTrack
{
    
    struct SampleProps
    {
        // time difference from start of samples
        // to next potential playback 
        uint64_t    frequency;
        uint32_t    probability;
        float       volumeMultiplier;
    };

public:
    EngineTrack( std::vector<EngineSample*> samples ):
        m_samples(std::move(samples))
    {
    }
    const std::vector<EngineSample*>& getSamples() const
    {
        return m_samples;
    }
    const std::vector<EngineGroup>& getGroups() const
    {
        return m_groups;
    }
private:
    std::vector<EngineSample*> m_samples;
    std::vector<EngineGroup> m_groups;
    std::vector<SampleProps> m_sampleProps;
};

