
#include <private/akPGTATrackLoader.h>
#include <private/akPGTATrack.h>
#include <private/akPGTASample.h>
#include <public/schema/track_generated.h>
#include <public/schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include <vector>

static PGTATrack* LoadBinaryTrack(const uint8_t* src, const size_t length, PGTATrack* track);
static PGTATrack* LoadAsciiTrack(const char* src, const size_t length, PGTATrack* track);
static PGTATrack* InitTrackData(PGTATrack* const track, const PGTASchema::Track* t);

static const size_t MAX_TRACK_LEN = (1 << 16);

PGTATrack* PGTATrackLoader::LoadTrack(const char* src, const size_t length, PGTATrack* track)
{
    if (!src || length > MAX_TRACK_LEN || !track)
    {
        return nullptr;
    }

    if (PGTASchema::TrackBufferHasIdentifier(src))
    {
        return LoadBinaryTrack(reinterpret_cast<const uint8_t*>(src), length, track);
    }
    else
    {
        return LoadAsciiTrack(src, length, track);
    }
}

static PGTATrack* LoadBinaryTrack(const uint8_t* src, const size_t length, PGTATrack* track)
{
    flatbuffers::Verifier verifier(src, length);
    if (!PGTASchema::VerifyTrackBuffer(verifier))
    {
        return nullptr;
    }

    const PGTASchema::Track* trackSchema = PGTASchema::GetTrack(src);

    return InitTrackData(track, trackSchema);
}

static PGTATrack* LoadAsciiTrack(const char* src, const size_t length, PGTATrack* track)
{
    flatbuffers::Parser parser;
    if (!parser.Parse(PGTASchemaHeader::TRACK_FBS) || !parser.Parse(src))
    {
        return nullptr;
    }

    const PGTASchema::Track* trackSchema = PGTASchema::GetTrack(parser.builder_.GetBufferPointer());

    return InitTrackData(track, trackSchema);
}

static PGTATrack* InitTrackData(PGTATrack* const track, const PGTASchema::Track* t)
{
    auto s = t->samples();

    int len = s->size();

    if (len == 0) { return nullptr; }

    std::vector<PGTATrackSample> samples;
    for (int i = 0; i < len; ++i)
    {
        auto schemaSample = s->Get(i);
        PGTATrackSample sample;
        sample.sampleName = schemaSample->name()->c_str();
        sample.startTime = schemaSample->startTime();
        sample.frequency = schemaSample->frequency();
        sample.probability = (uint32_t)(schemaSample->probability());

        int numGroups = schemaSample->groupIds()->size();
        for (int j = 0; j < numGroups; ++j)
        {
            auto group = schemaSample->groupIds()->Get(j);

            int8_t groupIdBuffer[NUM_UUID_BYTES];
            if (group->uuid()->size() != NUM_UUID_BYTES) { continue; }
            memcpy(groupIdBuffer, group->uuid()->Data(), NUM_UUID_BYTES);

            PGTAUUID *groupId = new PGTAUUID(groupIdBuffer);
            sample.groups.emplace_back(groupId);
        }

        samples.emplace_back(sample);
    }

    track->setSamples(samples);
    return track;
}
