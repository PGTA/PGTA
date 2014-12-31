
#pragma once

#include <stdint.h>
#include <set>

struct PGTAConfig;
class PGTATrack;
class PGTAContextImpl;

class PGTADeviceImpl
{
public:
    PGTADeviceImpl();
    ~PGTADeviceImpl();

    bool Initialize();
    void Shutdown();

    int32_t CreateTracks(const int32_t numTracks, const char** trackSourcesIn, PGTATrack** tracksOut);
    void FreeTracks(const int32_t numTracks, PGTATrack** tracksIn);

    PGTAContextImpl* CreateContext(const PGTAConfig& config);
    void DestroyContext(PGTAContextImpl* pgtaContext);

private:
    std::set<PGTAContextImpl*> m_contexts;
    std::set<PGTATrack*> m_tracks;
};
