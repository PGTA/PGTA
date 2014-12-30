
#include <public/akPGTAc.h>
#include <private/akPGTADeviceImpl.h>
#include <private/akPGTAContextImpl.h>
#include <private/akPGTATrack.h>

union PGTADeviceUnion
{
    HPGTADevice deviceHandle;
    PGTADeviceImpl* pgtaDevice;
};

union PGTAContextUnion
{
    HPGTAContext deviceHandle;
    PGTAContextImpl* pgtaDevice;
};

union PGTATrackUnion
{
    HPGTATrack trackHandle;
    PGTATrack* pgtaTrack;
};

union PGTATrackArrayUnion
{
    HPGTATrack* trackHandle;
    PGTATrack** pgtaTrack;
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
    return temp.deviceHandle;
}

void pgtaDestroyDevice(HPGTADevice device)
{
    if (device)
    {
        PGTADeviceUnion temp;
        temp.deviceHandle = device;
        temp.pgtaDevice->Shutdown();

        delete temp.pgtaDevice;
    }
}

int pgtaCreateTracks(HPGTADevice device, const int numTracks,
                     const char** trackSourcesIn, HPGTATrack* tracksOut)
{
    if (!device || (numTracks <= 0) || !trackSourcesIn || !tracksOut)
    {
        return 0;
    }

    PGTATrackArrayUnion tracks;
    tracks.trackHandle = tracksOut;

    PGTADeviceUnion temp;
    temp.deviceHandle = device;
    return temp.pgtaDevice->CreateTracks(numTracks, trackSourcesIn, tracks.pgtaTrack);
}

void pgtaFreeTracks(HPGTADevice device, const int numTracks, HPGTATrack* tracksIn)
{
    PGTATrackArrayUnion tracks;
    tracks.trackHandle = tracksIn;

    PGTADeviceUnion temp;
    temp.deviceHandle = device;
    temp.pgtaDevice->FreeTracks(numTracks, tracks.pgtaTrack);
}

PGTA_API int32_t         pgtaGetTrackData(HPGTATrack track);
PGTA_API void            pgtaBindTrackSamples();

PGTA_API HPGTAContext    pgtaCreateContext(HPGTADevice device, const PGTAConfig &config);
PGTA_API void            pgtaDestroyContext(HPGTAContext context);
PGTA_API void            pgtaBindTrack(HPGTAContext context, HPGTATrack track);
PGTA_API PGTABuffer*     pgtaUpdate(HPGTAContext context, float delta, int32_t* numOutputBuffers);
PGTA_API PGTABuffer*     pgtaGetOutputBuffers(int32_t* numOutputBuffers);
