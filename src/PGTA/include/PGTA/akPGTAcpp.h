
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

        inline int CreateTracks(const int numTracks, const char** trackSourcesIn,
                                const size_t* trackSourceLengths, HPGTATrack* tracksOut);
        inline void FreeTracks(const int numTracks, HPGTATrack* tracksIn);

        inline PGTAContext CreateContext(const PGTAConfig &config);
        inline void DestroyContext(PGTAContext& context);
        
    private:
        HPGTADevice m_pgtaDevice;

        std::set<HPGTAContext> m_contexts;
        std::vector<HPGTATrack> m_loadedTrackHandles;
    };
    
    class PGTAContext
    {
    public:
        explicit inline PGTAContext(HPGTAContext context = nullptr);
        inline PGTAContext(const PGTAContext& other);
        inline ~PGTAContext();
        
        inline PGTABuffer Update(const float deltaSeconds);
        inline PGTABuffer GetOutputBuffer();
        inline void BindTrack(HPGTATrack track);
        inline void Transition(HPGTATrack track, const float percentAmount, const float durationSeconds);

    protected:
        friend PGTAContext PGTADevice::CreateContext(const PGTAConfig &config);
        friend void PGTADevice::DestroyContext(PGTAContext& context);

    private:
        HPGTAContext m_pgtaContext;
    };

    inline void BindTrackSample(HPGTATrack track, const int32_t id, const uint8_t* audioData, const size_t audioDataNumBytes)
    {
        pgtaBindTrackSample(track, id, audioData, audioDataNumBytes);
    }
}

#include "akPGTADevice.inl"
#include "akPGTAContext.inl"

#endif // AK_PGTA_CPP_H
