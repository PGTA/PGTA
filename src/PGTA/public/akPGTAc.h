
#pragma once

#include "public/akPGTATypes.h"

#ifdef __cplusplus
extern "C" {
#endif

HPGTADevice     PGTACreateDevice();
void            PGTADestroyDevice(HPGTADevice device);

HPGTATrack      PGTALoadTrack(HPGTADevice device, const char* trackName);
int32_t         PGTAGetTrackData(HPGTATrack track, 
void            PGTABindTrackSamples();

HPGTAContext    PGTACreateContext(HPGTADevice device, const PGTAConfig &config);
void            PGTADestroyContext(HPGTAContext context);
void            PGTABindTrack(HPGTAContext context, HPGTATrack track);
PGTABuffer*     PGTAUpdate(HPGTAContext context, float delta, int32_t* numOutputBuffers);
PGTABuffer*     PGTAGetOutputBuffers(int32_t* numOutputBuffers);

#ifdef __cplusplus
} // extern "C"
#endif
