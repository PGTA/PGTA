
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
    PGTADeviceUnion temp;
    temp.pgtaDevice = new PGTADeviceImpl();
    if (!temp.pgtaDevice->Initialize())
    {
        delete temp.pgtaDevice;
        temp.pgtaDevice = nullptr;
    }
    return temp.handle;
}

void pgtaDestroyDevice(HPGTADevice device)
{
    if (!device)
    {
        return;
    }

    PGTADeviceUnion temp;
    temp.handle = device;
    temp.pgtaDevice->Shutdown();

    delete temp.pgtaDevice;
}

int pgtaCreateTracks(HPGTADevice device, const int numTracks, const char** trackSourcesIn,
                     const size_t* trackSourceLengths, HPGTATrack* tracksOut)
{
    if (!device)
    {
        return 0;
    }

    PGTATrackArrayUnion tracks;
    tracks.handle = tracksOut;

    PGTADeviceUnion temp;
    temp.handle = device;
    return temp.pgtaDevice->CreateTracks(numTracks, trackSourcesIn,
                                         trackSourceLengths, tracks.pgtaTracks);
}

void pgtaFreeTracks(HPGTADevice device, const int numTracks, HPGTATrack* tracksIn)
{
    if (!device)
    {
        return;
    }

    PGTATrackArrayUnion tracks;
    tracks.handle = tracksIn;

    PGTADeviceUnion temp;
    temp.handle = device;
    temp.pgtaDevice->FreeTracks(numTracks, tracks.pgtaTracks);
}

PGTATrackData pgtaGetTrackData(HPGTATrack track)
{
    if (!track)
    {
        return PGTATrackData();
    }

    PGTATrackUnion temp;
    temp.handle = track;

    return temp.pgtaTrack->GetTrackData(temp.handle);
}

void pgtaFreeTrackData(PGTATrackData trackData)
{
    PGTATrackUnion temp;
    temp.handle = trackData.trackHandle;

    if (!temp.pgtaTrack)
    {
        return;
    }

    temp.pgtaTrack->FreeTrackData();
}

void pgtaBindTrackSample(HPGTATrack track, const int32_t id, const uint8_t* audioData,
                         const size_t audioDataNumBytes)
{
    if (!track)
    {
        return;
    }

    PGTATrackUnion temp;
    temp.handle = track;

    int numSamples = (int)temp.pgtaTrack->GetNumSamples();
    auto* trackSamples = temp.pgtaTrack->GetSamplesForBinding();
    for (int i = 0; i < numSamples; ++i)
    {
        PGTATrackSample& trackSample = trackSamples->at(i);
        if (trackSample.id != id)
        {
            continue;
        }

        trackSample.audioData = audioData;
        trackSample.audioDataNumBytes = static_cast<uint32_t>(audioDataNumBytes);
        break;
    }
}

HPGTAContext pgtaCreateContext(HPGTADevice device, const PGTAConfig* config)
{
    if (!device || !config)
    {
        return nullptr;
    }

    PGTADeviceUnion devUnion;
    devUnion.handle = device;
    
    PGTAContextUnion contextUnion;
    contextUnion.pgtaContext = devUnion.pgtaDevice->CreateContext(*config);
    return contextUnion.handle;
}

void pgtaDestroyContext(HPGTADevice device, HPGTAContext context)
{
    if (!device || !context)
    {
        return;
    }

    PGTADeviceUnion devUnion;
    devUnion.handle = device;

    PGTAContextUnion contextUnion;
    contextUnion.handle = context;

    devUnion.pgtaDevice->DestroyContext(contextUnion.pgtaContext);
}

void pgtaBindTrack(HPGTAContext context, HPGTATrack track)
{
    if (!context || !track)
    {
        return;
    }

    PGTAContextUnion tempContext;
    tempContext.handle = context;

    PGTATrackUnion tempTrack;
    tempTrack.handle = track;

    tempContext.pgtaContext->BindTrack(tempTrack.pgtaTrack);
}

void pgtaTransition(HPGTAContext context, HPGTATrack track, const float percentAmount, const float durationSeconds)
{
    if (!context || !track)
    {
        return;
    }

    PGTAContextUnion tempContext;
    tempContext.handle = context;

    PGTATrackUnion tempTrack;
    tempTrack.handle = track;

    tempContext.pgtaContext->Transition(tempTrack.pgtaTrack, percentAmount, durationSeconds);
}

PGTABuffer pgtaUpdate(HPGTAContext context, float deltaSeconds)
{
    if (!context || deltaSeconds <= 0.0f)
    {
        return PGTABuffer();
    }

    PGTAContextUnion temp;
    temp.handle = context;
    return temp.pgtaContext->Update(deltaSeconds);
}

PGTABuffer pgtaGetOutputBuffer(HPGTAContext context)
{
    if (!context)
    {
        return PGTABuffer();
    }

    PGTAContextUnion temp;
    temp.handle = context;
    return temp.pgtaContext->GetOutputBuffer();
}

