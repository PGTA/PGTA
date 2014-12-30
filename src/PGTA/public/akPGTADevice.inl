
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
            m_pgtaDevice = PGTACreateDevice();
            return m_pgtaDevice != nullptr;
        }
        return true;
    }

    void PGTADevice::Destroy()
    {
        if (!m_pgtaDevice)
        {
            return;
        }

        for (auto& pair : m_loadedTracks)
        {
            PGTAFreeTrack(m_pgtaDevice, pair.second);
        }
        m_loadedTracks.clear();

        PGTADestroyDevice(m_pgtaDevice);
        m_pgtaDevice = nullptr;
    }

    HPGTATrack PGTADevice::LoadTrack(const char* trackName)
    {
        if (!m_pgtaDevice || !trackName)
        {
            return nullptr;
        }

        auto it = m_loadedTracks.find(trackName);
        if (it != m_loadedTracks.end())
        {
            return it->second;
        }

        auto track = PGTALoadTrack(m_pgtaDevice, trackName);
        if (!track)
        {
            return nullptr;
        }

        m_loadedTracks.emplace(trackName, track);
        return track;
    }

    HPGTAContext PGTADevice::PGTACreateContext(const PGTAConfig &config)
    {

    }
}
