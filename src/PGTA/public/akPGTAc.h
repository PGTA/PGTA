
#pragma once

#include <public/akPGTATypes.h>
#include <public/akPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

PGTA_API HPGTADevice     PGTACreateDevice();
PGTA_API void            PGTADestroyDevice(HPGTADevice device);

PGTA_API HPGTATrack      PGTALoadTrack(HPGTADevice device, const char* trackName);
PGTA_API void            PGTAFreeTrack(HPGTADevice device, HPGTATrack track);
PGTA_API int32_t         PGTAGetTrackData(HPGTATrack track);
PGTA_API void            PGTABindTrackSamples();

PGTA_API HPGTAContext    PGTACreateContext(HPGTADevice device, const PGTAConfig &config);
PGTA_API void            PGTADestroyContext(HPGTAContext context);
PGTA_API void            PGTABindTrack(HPGTAContext context, HPGTATrack track);
PGTA_API PGTABuffer*     PGTAUpdate(HPGTAContext context, float delta, int32_t* numOutputBuffers);
PGTA_API PGTABuffer*     PGTAGetOutputBuffers(int32_t* numOutputBuffers);

#ifdef __cplusplus
} // extern "C"
#endif
