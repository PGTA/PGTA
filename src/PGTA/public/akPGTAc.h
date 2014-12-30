
#pragma once

#include <public/akPGTATypes.h>
#include <public/akPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

PGTA_API HPGTADevice    pgtaCreateDevice();
PGTA_API void           pgtaDestroyDevice(HPGTADevice device);

PGTA_API int            pgtaCreateTracks(HPGTADevice device,
                                         const int numTracks,
                                         const char** trackSourcesIn,
                                         HPGTATrack* tracksOut);

PGTA_API void           pgtaFreeTracks(HPGTADevice device,
                                       const int numTracks,
                                       HPGTATrack* tracksIn);

PGTA_API int32_t        pgtaGetTrackData(HPGTATrack track);
PGTA_API void           pgtaBindTrackSamples();

PGTA_API HPGTAContext   pgtaCreateContext(HPGTADevice device, const PGTAConfig &config);
PGTA_API void           pgtaDestroyContext(HPGTAContext context);
PGTA_API void           pgtaBindTrack(HPGTAContext context, HPGTATrack track);
PGTA_API PGTABuffer*    pgtaUpdate(HPGTAContext context, float delta, int32_t* numOutputBuffers);
PGTA_API PGTABuffer*    pgtaGetOutputBuffers(int32_t* numOutputBuffers);

#ifdef __cplusplus
} // extern "C"
#endif
