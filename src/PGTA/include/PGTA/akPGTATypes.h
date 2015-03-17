
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
    const int16_t* samples;
    int32_t numSamples;
} PGTABuffer;

typedef struct PGTAConfig
{
    PGTAAudioDesc audioDesc;
    uint16_t beatsPerMinute;
    float mixAhead;
} PGTAConfig;

typedef struct PGTAGroupData
{
    const char* name;
    const char* uuid;
} PGTAGroupData;

typedef struct PGTARestrictionData
{
    const char* group1UUID;
    const char* group2UUID;
} PGTARestrictionData;

typedef struct PGTASampleData
{
    int16_t id;
    const char* sampleName;
    const char* defaultFile;
    float period;
    float periodDeviation;
    float startTime;
    float probability;
    float gain;
    const char* groupUUID;
} PGTASampleData;

typedef struct PGTATrackData
{
    HPGTATrack trackHandle;
    uint16_t numSamples;
    const PGTASampleData* samples;

    uint16_t numGroups;
    const PGTAGroupData* groups;

    uint16_t numRestrictions;
    const PGTARestrictionData* restrictions;

    int isMeasuredInBeats;
} PGTATrackData;

#undef DECLARE_HANDLE
