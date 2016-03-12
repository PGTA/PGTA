
#include <PGTA/akPGTAc.h>
#include "akPGTADeviceImpl.h"
#include "akPGTAContextImpl.h"
#include "akPGTATrack.h"
#include <memory>
#include <string.h>

union PGTADeviceUnion
{
    HPGTADevice handle;
    PGTADeviceImpl* pgtaDevice;
};

union PGTAContextUnion
{
    HPGTAContext handle;
    PGTAContextImpl* pgtaContext;
};

union PGTATrackUnion
{
    HPGTATrack handle;
    PGTATrack* pgtaTrack;
};

union PGTATrackArrayUnion
{
    HPGTATrack* handle;
    PGTATrack** pgtaTracks;
};

HPGTADevice pgtaCreateDevice()
{
    PGTADeviceImpl* pgtaDevice = new PGTADeviceImpl();
    if (!pgtaDevice->Initialize())
    {
        delete pgtaDevice;
        pgtaDevice = nullptr;
    }
    return reinterpret_cast<PGTADeviceUnion*>(&pgtaDevice)->handle;
}

void pgtaDestroyDevice(HPGTADevice device)
{
    if (!device)
    {
        return;
    }

    PGTADeviceImpl* pgtaDevice = reinterpret_cast<PGTADeviceUnion*>(&device)->pgtaDevice;
    pgtaDevice->Shutdown();
    delete pgtaDevice;
}

int pgtaCreateTracks(HPGTADevice device, const int numTracks, const char** trackSourcesIn,
                     const size_t* trackSourceLengths, HPGTATrack* tracksOut)
{
    if (!device)
    {
        return 0;
    }

    PGTADeviceImpl* pgtaDevice = reinterpret_cast<PGTADeviceUnion*>(&device)->pgtaDevice;
    PGTATrack** pgtaTracks = reinterpret_cast<PGTATrackArrayUnion*>(&tracksOut)->pgtaTracks;

    return pgtaDevice->CreateTracks(numTracks, trackSourcesIn,
                                    trackSourceLengths, pgtaTracks);
}

void pgtaFreeTracks(HPGTADevice device, const int numTracks, HPGTATrack* tracksIn)
{
    if (!device)
    {
        return;
    }

    PGTADeviceImpl* pgtaDevice = reinterpret_cast<PGTADeviceUnion*>(&device)->pgtaDevice;
    PGTATrack** pgtaTracks = reinterpret_cast<PGTATrackArrayUnion*>(&tracksIn)->pgtaTracks;

    pgtaDevice->FreeTracks(numTracks, pgtaTracks);
}

PGTATrackData pgtaGetTrackData(HPGTATrack track)
{
    if (!track)
    {
        return PGTATrackData();
    }

    PGTATrack* pgtaTrack = reinterpret_cast<PGTATrackUnion*>(&track)->pgtaTrack;

    return pgtaTrack->GetTrackData(track);
}

void pgtaFreeTrackData(PGTATrackData trackData)
{
    PGTATrack* pgtaTrack = reinterpret_cast<PGTATrackUnion*>(&trackData.trackHandle)->pgtaTrack;
    if (!pgtaTrack)
    {
        return;
    }

    pgtaTrack->FreeTrackData();
}

void pgtaBindTrackSample(HPGTATrack track, const int32_t id, const uint8_t* audioData,
                         const size_t audioDataNumBytes)
{
    if (!track)
    {
        return;
    }

    PGTATrack* pgtaTrack = reinterpret_cast<PGTATrackUnion*>(&track)->pgtaTrack;
    for (PGTATrackSample& trackSample : *pgtaTrack->GetSamplesForBinding())
    {
        if (trackSample.id == id)
        {
            trackSample.audioData = audioData;
            trackSample.audioDataNumBytes = static_cast<uint32_t>(audioDataNumBytes);
            break;
        }
    }
}

HPGTAContext pgtaCreateContext(HPGTADevice device, const PGTAConfig* config)
{
    if (!device || !config)
    {
        return nullptr;
    }

    PGTADeviceImpl* pgtaDevice = reinterpret_cast<PGTADeviceUnion*>(&device)->pgtaDevice;
    PGTAContextImpl* pgtaContext = pgtaDevice->CreateContext(*config);

    return reinterpret_cast<PGTAContextUnion*>(&pgtaContext)->handle;
}

void pgtaDestroyContext(HPGTADevice device, HPGTAContext context)
{
    if (!device || !context)
    {
        return;
    }

    PGTADeviceImpl* pgtaDevice = reinterpret_cast<PGTADeviceUnion*>(&device)->pgtaDevice;
    PGTAContextImpl* pgtaContext = reinterpret_cast<PGTAContextUnion*>(&context)->pgtaContext;

    pgtaDevice->DestroyContext(pgtaContext);
}

void pgtaBindTrack(HPGTAContext context, HPGTATrack track)
{
    if (!context || !track)
    {
        return;
    }

    PGTAContextImpl* pgtaContext = reinterpret_cast<PGTAContextUnion*>(&context)->pgtaContext;
    PGTATrack* pgtaTrack = reinterpret_cast<PGTATrackUnion*>(&track)->pgtaTrack;

    pgtaContext->BindTrack(pgtaTrack);
}

void pgtaTransition(HPGTAContext context, HPGTATrack track, const float percentAmount, const float durationSeconds)
{
    if (!context || !track)
    {
        return;
    }

    PGTAContextImpl* pgtaContext = reinterpret_cast<PGTAContextUnion*>(&context)->pgtaContext;
    PGTATrack* pgtaTrack = reinterpret_cast<PGTATrackUnion*>(&track)->pgtaTrack;

    pgtaContext->Transition(pgtaTrack, percentAmount, durationSeconds);
}

PGTABuffer pgtaUpdate(HPGTAContext context, float deltaSeconds)
{
    if (!context || deltaSeconds <= 0.0f)
    {
        return PGTABuffer();
    }

    PGTAContextImpl* pgtaContext = reinterpret_cast<PGTAContextUnion*>(&context)->pgtaContext;

    return pgtaContext->Update(deltaSeconds);
}

PGTABuffer pgtaGetOutputBuffer(HPGTAContext context)
{
    if (!context)
    {
        return PGTABuffer();
    }

    PGTAContextImpl* pgtaContext = reinterpret_cast<PGTAContextUnion*>(&context)->pgtaContext;

    return pgtaContext->GetOutputBuffer();
}

