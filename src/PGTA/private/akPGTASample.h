
#pragma once

#include <private/akPGTAUUID.h>
#include <stdint.h>
#include <vector>
#include <string>

struct PGTATrackSample
{
    std::string sampleName;
    uint64_t frequency;
    uint64_t startTime;
    float probability;
    float volumeMultiplier;

    std::vector<PGTAUUID> groups;
    //const int16_t* audioData;
};
