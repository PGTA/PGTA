
#include "akPGTADeviceImpl.h"
#include "akPGTAContextImpl.h"
#include "akPGTATrack.h"
#include "akPGTATrackLoader.h"
#include <PGTA/akPGTATypes.h>
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
    for (PGTAContextImpl* context : m_contexts)
    {
        context->Shutdown();
        delete context;
    }

    for (PGTATrack* track : m_tracks)
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
    int32_t numCreatedTracks = numTracks;
    for (int32_t i = 0; i < numTracks; ++i)
    {
        const char* source = trackSourcesIn[i];
        const size_t length = trackSourceLengths[i];

        PGTATrack* newTrack = new PGTATrack();
        track.reset(newTrack);
        if (!PGTATrackLoader::LoadTrack(source, length, newTrack))
        {
            // error
            tracksOut[i] = nullptr;
            --numCreatedTracks;
            continue;
        }

        tracksOut[i] = newTrack;
        m_tracks.emplace(newTrack);
        track.release();
    }
    return numCreatedTracks;
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
    PGTAContextImpl* context = new PGTAContextImpl();
    if (context && context->Initialize(config))
    {
        m_contexts.emplace(context);
        return context;
    }
    delete context;
    return nullptr;
}

void PGTADeviceImpl::DestroyContext(PGTAContextImpl* pgtaContext)
{
    auto it = m_contexts.find(pgtaContext);
    if (pgtaContext && (it != m_contexts.end()))
    {
        m_contexts.erase(it);
        pgtaContext->Shutdown();
        delete pgtaContext;
    }
}
