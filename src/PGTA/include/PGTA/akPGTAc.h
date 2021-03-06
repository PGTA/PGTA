
#pragma once

#include "akPGTATypes.h"
#include "akPlatform.h"

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

PGTA_API PGTATrackData  pgtaGetTrackData(HPGTATrack track);
PGTA_API void           pgtaFreeTrackData(PGTATrackData trackData);

PGTA_API void           pgtaBindTrackSample(HPGTATrack track,
                                            const int32_t id,
                                            const uint8_t* audioData,
                                            const size_t audioDataNumBytes);

PGTA_API HPGTAContext   pgtaCreateContext(HPGTADevice device, const PGTAConfig* config);
PGTA_API void           pgtaDestroyContext(HPGTADevice device, HPGTAContext context);

PGTA_API void           pgtaBindTrack(HPGTAContext context, HPGTATrack track);
PGTA_API PGTABuffer     pgtaUpdate(HPGTAContext context, const float deltaSeconds);
PGTA_API void           pgtaTransition(HPGTAContext context,
                                       HPGTATrack track,
                                       const float percentAmount,
                                       const float durationSeconds);
PGTA_API PGTABuffer     pgtaGetOutputBuffer(HPGTAContext context);

#ifdef __cplusplus
} // extern "C"
#endif
