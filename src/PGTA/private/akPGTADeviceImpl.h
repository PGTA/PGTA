
#pragma once

#include <private/akPGTATrack.h>
#include <set>

class PGTADeviceImpl
{
public:
    PGTADeviceImpl() {}
    ~PGTADeviceImpl() {}

    bool Initialize() { return true; }
    void Shutdown() {}

    int CreateTracks(const int numTracks, const char** trackSourcesIn, PGTATrack** tracksOut)
    {
        for (int i = 0; i < numTracks; ++i)
        {
            PGTATrack* track = new PGTATrack();
            //load track from json source
            tracksOut[i] = track;
            m_tracks.emplace(track);
        }
        return numTracks;
    }

    void FreeTracks(const int numTracks, PGTATrack** tracksIn)
    {
        for (int i = 0; i < numTracks; ++i)
        {
            m_tracks.erase(tracksIn[i]);
        }
    }

private:
    std::set<PGTATrack*> m_tracks;
};
