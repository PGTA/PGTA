
#ifndef AK_PGTA_CPP_H
#define AK_PGTA_CPP_H

#include "akPGTAc.h"
#include <string>
#include <unordered_map>

namespace PGTA
{
    class PGTADevice
    {
    public:
        inline PGTADevice();
        inline ~PGTADevice();

        inline bool Initialize();
        inline void Destroy();

        inline HPGTATrack LoadTrack(const char* trackName);
        inline HPGTAContext PGTACreateContext(const PGTAConfig &config);
        
    private:
        HPGTADevice m_pgtaDevice;
        std::unordered_map<std::string, HPGTATrack> m_loadedTracks;
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
