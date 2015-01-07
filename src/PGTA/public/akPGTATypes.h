
#pragma once

#include <stdint.h>
#include <stddef.h>

#define DECLARE_HANDLE(type) typedef struct J##type { int unused; }* type

DECLARE_HANDLE(HPGTADevice);
DECLARE_HANDLE(HPGTAContext);
DECLARE_HANDLE(HPGTATrack);

struct PGTAAudioDesc
{
    uint32_t samplesPerSecond;
    uint16_t bytesPerSample;
    uint16_t channels;
};

struct PGTABuffer
{
    const struct PGTAAudioDesc* audioDesc;
    const char* samples;
    int32_t numSamples;
};

struct PGTAConfig
{
    struct PGTAAudioDesc audioDesc;

    uint16_t numBuffers;
    uint16_t bufferSizeInSamples;
};

struct PGTATrackSample
{
    const char* sampleName;
    uint64_t frequency;
    uint64_t startTime;
    uint32_t probability;
    float volumeMultiplier;
    
    int16_t sampleId;
};

struct PGTATrackData
{
};

#undef DECLARE_HANDLE
