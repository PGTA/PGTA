
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
        const int numTracks = m_loadedTrackHandles.size();
        pgtaFreeTracks(m_pgtaDevice, numTracks, m_loadedTrackHandles.data());
        m_loadedTrackHandles.clear();

        pgtaDestroyDevice(m_pgtaDevice);
        m_pgtaDevice = nullptr;
    }

    int PGTADevice::CreateTracks(const int numTracks, const char** trackSourceIn, HPGTATrack* tracksOut)
    {
        auto numLoadedTracks = pgtaCreateTracks(m_pgtaDevice, numTracks, trackSourceIn, tracksOut);
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
        return PGTAContext(pgtaCreateContext(m_pgtaDevice, config));
    }

    void PGTADevice::DestroyContext(PGTAContext &context)
    {
        pgtaDestroyContext(m_pgtaDevice, context.m_pgtaContext);
    }
}
