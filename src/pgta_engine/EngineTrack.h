
#pragma once

#include "EngineSample.h"
#include "EngineGroup.h"
#include <vector>
#include <map>
#include <cassert>

class EngineTrack
{
public:
	EngineTrack(std::vector<EngineSample> samples, std::vector<EngineGroup> groups, uint16_t instance, const char* trackName):
        m_samples(std::move(samples)),
		m_groups(std::move(groups)),
        m_instance(instance),
        m_name(trackName)
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

    const uint16_t getInstance() const
    {
        return m_instance;
    }

    const char* getName() const
    {
        return m_name;
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
    const uint16_t m_instance;
    const char* m_name;
    std::vector<EngineSample> m_samples;
    std::map<uint16_t, uint16_t> m_sampleGroups;
    std::vector<EngineGroup> m_groups;
};
