
#pragma once

#include "EngineSample.h"
#include "EngineGroup.h"
#include <vector>
#include <map>
#include <cassert>
#include <iostream>

class EngineTrack
{
public:
	EngineTrack(std::vector<EngineSample> samples, std::vector<EngineGroup> groups):
        m_samples(std::move(samples)),
		m_groups(std::move(groups))
    {
        int numGroups = (int)m_groups.size();
        for (int i = 0; i < numGroups; ++i)
        {
            for (auto sample : m_groups[i].getSampleIDs())
            {
                m_sampleGroups[sample] = i;
            }
        }
    }

    const std::vector<EngineSample>& getSamples() const
    {
        return m_samples;
    }

    const std::map<uint16_t, uint16_t>& getSampleGroups() const
    {
        return m_sampleGroups;
    }

    const std::vector<EngineGroup>& getGroups() const
    {
        return m_groups;
    }

    const EngineSample& GetEngineSample(int index) const
    {
        assert((int)m_samples.size() > index);
        return m_samples[index];
    }

private:
    std::vector<EngineSample> m_samples;
    std::map<uint16_t, uint16_t> m_sampleGroups;
    std::vector<EngineGroup> m_groups;
};
