
#include <private/akPGTADeviceImpl.h>
#include <private/akPGTAContextImpl.h>
#include <private/akPGTATrack.h>
#include <public/akPGTATypes.h>

PGTADeviceImpl::PGTADeviceImpl()
{
}

PGTADeviceImpl::~PGTADeviceImpl()
{
    Shutdown();
}

bool PGTADeviceImpl::Initialize()
{
    return true;
}

void PGTADeviceImpl::Shutdown()
{
    for (auto context : m_contexts)
    {
        context->Shutdown();
        delete context;
    }

    for (auto track : m_tracks)
    {
        delete track;
    }

    m_contexts.clear();
    m_tracks.clear();
}

int32_t PGTADeviceImpl::CreateTracks(const int32_t numTracks, const char** trackSourcesIn, PGTATrack** tracksOut)
{
    for (int32_t i = 0; i < numTracks; ++i)
    {
        PGTATrack* track = new PGTATrack();
        //load track from json source
        tracksOut[i] = track;
        m_tracks.emplace(track);
    }
    return numTracks;
}

void PGTADeviceImpl::FreeTracks(const int32_t numTracks, PGTATrack** tracksIn)
{
    for (int32_t i = 0; i < numTracks; ++i)
    {
        m_tracks.erase(tracksIn[i]);
    }
}

PGTAContextImpl* PGTADeviceImpl::CreateContext(const PGTAConfig& config)
{
    auto context = new PGTAContextImpl();
    if (context->Initialize())
    {
        m_contexts.emplace(context);
    }
    else
    {
        delete context;
        context = nullptr;
    }
    return context;
}

void PGTADeviceImpl::DestroyContext(PGTAContextImpl* pgtaContext)
{

}
