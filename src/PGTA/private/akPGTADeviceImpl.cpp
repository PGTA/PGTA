
#include <private/akPGTADeviceImpl.h>
#include <private/akPGTAContextImpl.h>
#include <private/akPGTATrack.h>
#include <private/akPGTATrackLoader.h>
#include <public/akPGTATypes.h>
#include <memory>

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

int32_t PGTADeviceImpl::CreateTracks(const int32_t numTracks, const char** trackSourcesIn,
                                     const size_t* trackSourceLengths, PGTATrack** tracksOut)
{
    std::unique_ptr<PGTATrack> track;
    for (int32_t i = 0; i < numTracks; ++i)
    {
        const char* source = trackSourcesIn[i];
        const size_t length = trackSourceLengths[i];

        PGTATrack* newTrack = new PGTATrack();
        track.reset(newTrack);
        if (!PGTATrackLoader::LoadTrack(source, length, newTrack))
        {
            // error
        }

        tracksOut[i] = newTrack;
        m_tracks.emplace(newTrack);
        track.release();
    }
    return numTracks;
}

void PGTADeviceImpl::FreeTracks(const int32_t numTracks, PGTATrack** tracksIn)
{
    for (int32_t i = 0; i < numTracks; ++i)
    {
        PGTATrack* track = tracksIn[i];
        m_tracks.erase(track);
        delete track;
    }
}

PGTAContextImpl* PGTADeviceImpl::CreateContext(const PGTAConfig& config)
{
    auto context = new PGTAContextImpl();
    if (context->Initialize(config))
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
    pgtaContext->Shutdown();
    delete pgtaContext;
}
