
#pragma once

class PGTATrack;

namespace PGTATrackLoader
{
    PGTATrack* LoadTrack(const char* src, const size_t length, PGTATrack* track);
}
