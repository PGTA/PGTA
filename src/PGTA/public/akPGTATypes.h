
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <public/akPGTAUUID.h>

#define DECLARE_HANDLE(type) typedef struct J##type { int unused; }* type

DECLARE_HANDLE(HPGTADevice);
DECLARE_HANDLE(HPGTAContext);
DECLARE_HANDLE(HPGTATrack);

typedef struct PGTAAudioDesc
{
    uint32_t samplesPerSecond;
    uint16_t bytesPerSample;
    uint16_t channels;
} PGTAAudioDesc;

typedef struct PGTABuffer
{
    const PGTAAudioDesc* audioDesc;
    const char* samples;
    int32_t numSamples;
} PGTABuffer;

typedef struct PGTAConfig
{
    PGTAAudioDesc audioDesc;

    uint16_t numBuffers;
    uint16_t bufferSizeInSamples;
} PGTAConfig;

typedef struct PGTATrackSample
{
    char* sampleName;
    int16_t id;
    char* defaultFile;
    uint64_t frequency;
    uint64_t startTime;
    float probability;
    float volumeMultiplier;

    uint16_t numGroups;
    PGTAUUID* groups;
} PGTATrackSample;

typedef struct PGTATrackData
{
    uint16_t numSamples;
    PGTATrackSample* samples;

} PGTATrackData;

#undef DECLARE_HANDLE
