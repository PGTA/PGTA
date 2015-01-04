
#ifndef AK_PGTA_CPP_H
#error "donut include pls"
#endif

namespace PGTA
{
    PGTADevice::PGTADevice():
        m_pgtaDevice(nullptr)
    {
    }

    PGTADevice::~PGTADevice()
    {
        Destroy();
    }

    bool PGTADevice::Initialize()
    {
        if (!m_pgtaDevice)
        {
            m_pgtaDevice = pgtaCreateDevice();
            return m_pgtaDevice != nullptr;
        }
        return true;
    }

    void PGTADevice::Destroy()
    {
        // pgtaDestroyDevice takes care of deallocation
        m_contexts.clear();
        m_loadedTrackHandles.clear();

        pgtaDestroyDevice(m_pgtaDevice);
        m_pgtaDevice = nullptr;
    }

    int PGTADevice::CreateTracks(const int numTracks, const char** trackSourceIn,
                                 const size_t* trackSourceLengths, HPGTATrack* tracksOut)
    {
        auto numLoadedTracks = pgtaCreateTracks(m_pgtaDevice, numTracks, trackSourceIn,
                                                trackSourceLengths, tracksOut);
        if (numLoadedTracks > 0)
        {
            m_loadedTrackHandles.insert(m_loadedTrackHandles.end(), tracksOut, tracksOut + numTracks);
        }
        return numLoadedTracks;
    }

    void PGTADevice::FreeTracks(const int numTracks, HPGTATrack* tracksIn)
    {
        pgtaFreeTracks(m_pgtaDevice, numTracks, tracksIn);
    }

    PGTAContext PGTADevice::CreateContext(const PGTAConfig &config)
    {
        HPGTAContext contextHandle = pgtaCreateContext(m_pgtaDevice, config);
        if (contextHandle)
        {
            m_contexts.emplace(contextHandle);
        }
        return PGTAContext(contextHandle);
    }

    void PGTADevice::DestroyContext(PGTAContext &context)
    {
        HPGTAContext contextHandle = context.m_pgtaContext;
        if (m_contexts.find(contextHandle) != m_contexts.end())
        {
            pgtaDestroyContext(m_pgtaDevice, contextHandle);
            m_contexts.erase(contextHandle);
        }
    }
}
