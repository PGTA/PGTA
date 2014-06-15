
#pragma once

#include "EngineSample.h"
#include "EngineGroup.h"
#include <vector>

class EngineTrack
{
public:
	EngineTrack( std::vector<EngineSample> samples, std::vector<EngineGroup> groups ) :
        m_samples(std::move(samples)),
		m_groups(std::move(groups))
    {
    }
    const std::vector<EngineSample>& getSamples() const
    {
        return m_samples;
    }
    const std::vector<EngineGroup>& getGroups() const
    {
        return m_groups;
    }
private:
    std::vector<EngineSample> m_samples;
    std::vector<EngineGroup> m_groups;
};

