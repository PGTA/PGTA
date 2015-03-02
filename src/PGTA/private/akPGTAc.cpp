
#include <public/akPGTAc.h>
#include <private/akPGTADeviceImpl.h>
#include <private/akPGTAContextImpl.h>
#include <private/akPGTATrack.h>
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

PGTATrackData* pgtaGetTrackData(HPGTATrack track)
{
    PGTATrackUnion trackUnion;
    trackUnion.handle = track;

    PGTATrack* tempTrack = trackUnion.pgtaTrack;
    PGTATrackData* data = tempTrack->GetTrackData(trackUnion.handle);

    return data;
}

void pgtaFreeTrackData(PGTATrackData* trackData)
{

    PGTATrackUnion trackUnion;
    trackUnion.handle = trackData->trackHandle;
    
    PGTATrack* track = trackUnion.pgtaTrack;
    track->FreeTrackData();
}

void pgtaBindTrackSamples()
{
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
}

PGTABuffer* pgtaUpdate(HPGTAContext context, float delta, int32_t* numOutputBuffers)
{
    if (!context || delta <= 0.0f)
    {
        return nullptr;
    }

    PGTAContextUnion temp;
    temp.handle = context;
    return temp.pgtaContext->Update(delta, numOutputBuffers);
}

PGTABuffer* pgtaGetOutputBuffers(HPGTAContext context, int32_t* numOutputBuffers)
{
    if (!context)
    {
        return nullptr;
    }

    PGTAContextUnion temp;
    temp.handle = context;
    return temp.pgtaContext->GetOutputBuffers(numOutputBuffers);
}
