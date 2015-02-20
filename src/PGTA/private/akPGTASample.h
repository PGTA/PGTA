
#pragma once

#include <private/akPGTAUUID.h>
#include <stdint.h>
#include <vector>

struct PGTATrackSample
{
    const char* sampleName;
    uint64_t frequency;
    uint64_t startTime;
    uint32_t probability;
    float volumeMultiplier;

    std::vector<PGTAUUID> groups;
    //const int16_t* audioData;
};
