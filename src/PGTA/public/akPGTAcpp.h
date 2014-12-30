
#ifndef AK_PGTA_CPP_H
#define AK_PGTA_CPP_H

#include "akPGTAc.h"
#include <string>
#include <vector>

namespace PGTA
{
    class PGTADevice
    {
    public:
        inline PGTADevice();
        inline ~PGTADevice();

        inline bool Initialize();
        inline void Destroy();

        inline int CreateTracks(const int numTracks, const char** trackSourcesIn, HPGTATrack* tracksOut);
        inline void FreeTracks(const int numTracks, HPGTATrack* tracksIn);

        inline HPGTAContext PGTACreateContext(const PGTAConfig &config);
        
    private:
        HPGTADevice m_pgtaDevice;

        std::vector<HPGTATrack> m_loadedTrackHandles;
    };
    
    class PGTAContext
    {
    public:
    private:
    };
}

#include <public/akPGTADevice.inl>
#include <public/akPGTAContext.inl>

#endif // AK_PGTA_CPP_H
