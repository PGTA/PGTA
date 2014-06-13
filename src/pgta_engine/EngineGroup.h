
#pragma once

#include <cstdint>
#include <vector> 

class EngineGroup
{
public:
    EngineGroup( std::vector<uint16_t> sampleIDs ) :
        m_sampleIDs(std::move(sampleIDs))
    {
    }
    const std::vector<uint16_t>& getSampleIDs() const
    {
        return m_sampleIDs;
    }
private:
    std::vector<uint16_t> m_sampleIDs; 
};

