
#pragma once

#include <stdint.h>
#include <stddef.h>

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

typedef struct PGTAGroupData
{
    char* name;
    char* uuid;
} PGTAGroupData;

typedef struct PGTARestrictionData
{
    char* group1UUID;
    char* group2UUID;
} PGTARestrictionData;

typedef struct PGTASampleData
{
    char* sampleName;
    char* defaultFile;
    uint64_t frequency;
    uint64_t startTime;
    float probability;
    float volumeMultiplier;
    char* groupUUID;
} PGTASampleData;

typedef struct PGTATrackData
{
    HPGTATrack trackHandle;
    uint16_t numSamples;
    PGTASampleData* samples;

    uint16_t numGroups;
    PGTAGroupData* groups;

    uint16_t numRestrictions;
    PGTARestrictionData* restrictions;

} PGTATrackData;

#undef DECLARE_HANDLE
