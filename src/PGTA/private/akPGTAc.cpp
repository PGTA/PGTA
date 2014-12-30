
#include <public/akPGTAc.h>

HPGTADevice PGTACreateDevice()
{
    return nullptr;
}

void PGTADestroyDevice(HPGTADevice device)
{
}

HPGTATrack PGTALoadTrack(HPGTADevice device, const char* trackName)
{
    return nullptr;
}

void PGTAFreeTrack(HPGTADevice device, HPGTATrack track)
{
}

PGTA_API int32_t         PGTAGetTrackData(HPGTATrack track);
PGTA_API void            PGTABindTrackSamples();

PGTA_API HPGTAContext    PGTACreateContext(HPGTADevice device, const PGTAConfig &config);
PGTA_API void            PGTADestroyContext(HPGTAContext context);
PGTA_API void            PGTABindTrack(HPGTAContext context, HPGTATrack track);
PGTA_API PGTABuffer*     PGTAUpdate(HPGTAContext context, float delta, int32_t* numOutputBuffers);
PGTA_API PGTABuffer*     PGTAGetOutputBuffers(int32_t* numOutputBuffers);
