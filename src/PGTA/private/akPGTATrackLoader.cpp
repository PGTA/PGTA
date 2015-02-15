
#include <private/akPGTATrackLoader.h>
#include <private/akPGTATrack.h>
#include <public/schema/track_generated.h>
#include <public/schema/track.fbs.h>
#include <flatbuffers/idl.h>

static PGTATrack* LoadBinaryTrack(const char* src, const size_t length, PGTATrack* track);
static PGTATrack* LoadAsciiTrack(const char* src, const size_t length, PGTATrack* track);

PGTATrack* PGTATrackLoader::LoadTrack(const char* src, const size_t length, PGTATrack* track)
{
    if (flatbuffers::BufferHasIdentifier(src, "PGTA"))
    {
        return LoadBinaryTrack(src, length, track);
    }
    else
    {
        return LoadAsciiTrack(src, length, track);
    }
}

static PGTATrack* LoadBinaryTrack(const char* src, const size_t length, PGTATrack* track)
{
    return nullptr;
}

static PGTATrack* LoadAsciiTrack(const char* src, const size_t length, PGTATrack* track)
{
    flatbuffers::Parser parser;
    if (!parser.Parse(PGTASchemaHeader::TRACK_FBS) || !parser.Parse(src))
    {
        return nullptr;
    }

    return nullptr;
}
