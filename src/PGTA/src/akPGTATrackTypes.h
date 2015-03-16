
#pragma once

#include "akPGTAUUID.h"
#include <PGTA/akPGTATypes.h>
#include <memory>
#include <vector>
#include <string>

struct PGTATrackGroup
{
    std::string name;
    std::string uuid;
};

struct PGTATrackSample
{
    int16_t id;
    std::string sampleName;
    std::string defaultFile;
    float period;
    float periodDeviation;
    float startTime;
    float probability;
    float volume;

    std::string group;

    uint32_t numSamples;
    const int16_t* audioData;
};

struct PGTACachedGroupData
{
    std::string name;
    std::string uuid;
};

struct PGTACachedRestrictionData
{
    std::string group1UUID;
    std::string group2UUID;
};

struct PGTACachedSampleData
{
    std::string sampleName;
    std::string defaultFile;
    std::string groupUUID;
};

struct PGTACachedTrackData
{
    HPGTATrack trackHandle;

    std::vector<PGTASampleData> samples;
    std::vector<PGTAGroupData> groups;
    std::vector<PGTARestrictionData> restrictions;
    
    std::vector<PGTACachedSampleData> cachedSamples;
    std::vector<PGTACachedGroupData> cachedGroups;
    std::vector<PGTACachedRestrictionData> cachedRestrictions;
};
