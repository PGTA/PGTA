
#ifndef AK_PGTA_CPP_H
#define AK_PGTA_CPP_H

#include "akPGTAc.h"
#include <set>
#include <vector>

namespace PGTA
{
    class PGTADevice;
    class PGTAContext;

    class PGTADevice
    {
    public:
        inline PGTADevice();
        inline ~PGTADevice();

        inline bool Initialize();
        inline void Destroy();

        inline int CreateTracks(const int numTracks, const char** trackSourcesIn, HPGTATrack* tracksOut);
        inline void FreeTracks(const int numTracks, HPGTATrack* tracksIn);

        inline PGTAContext CreateContext(const PGTAConfig &config);
        inline void DestroyContext(PGTAContext& context);
        
    private:
        HPGTADevice m_pgtaDevice;

        std::set<PGTAContext> m_contexts;
        std::vector<HPGTATrack> m_loadedTrackHandles;
    };
    
    class PGTAContext
    {
    public:
        inline PGTAContext(const PGTAContext& other);
        inline ~PGTAContext();
        
        inline PGTABuffer* Update(const float delta, int* numOutputBuffers);
        inline PGTABuffer* GetOutputBuffers(int32_t* numOutputBuffers);

    protected:
        friend PGTAContext PGTADevice::CreateContext(const PGTAConfig &config);
        friend void PGTADevice::DestroyContext(PGTAContext& context);
        explicit inline PGTAContext(HPGTAContext context);

    private:
        HPGTAContext m_pgtaContext;
    };
}

#include <public/akPGTADevice.inl>
#include <public/akPGTAContext.inl>

#endif // AK_PGTA_CPP_H
