
#pragma once

#include <public/akPGTATypes.h>
#include <public/akPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

PGTA_API HPGTADevice    pgtaCreateDevice();
PGTA_API void           pgtaDestroyDevice(HPGTADevice device);

PGTA_API int32_t        pgtaCreateTracks(HPGTADevice device,
                                         const int32_t numTracks,
                                         const char** trackSourcesIn,
                                         const size_t* trackSourceLengths,
                                         HPGTATrack* tracksOut);

PGTA_API void           pgtaFreeTracks(HPGTADevice device,
                                       const int32_t numTracks,
                                       HPGTATrack* tracksIn);

PGTA_API PGTATrackData pgtaGetTrackData(HPGTATrack track);
PGTA_API void pgtaFreeTrackData(PGTATrackData trackData);

PGTA_API void           pgtaBindTrackSamples();

PGTA_API HPGTAContext   pgtaCreateContext(HPGTADevice device, const PGTAConfig* config);
PGTA_API void           pgtaDestroyContext(HPGTADevice device, HPGTAContext context);

PGTA_API void           pgtaBindTrack(HPGTAContext context, HPGTATrack track);
PGTA_API PGTABuffer*    pgtaUpdate(HPGTAContext context, const float delta, int32_t* numOutputBuffers);
PGTA_API PGTABuffer*    pgtaGetOutputBuffers(HPGTAContext context, int32_t* numOutputBuffers);

#ifdef __cplusplus
} // extern "C"
#endif
