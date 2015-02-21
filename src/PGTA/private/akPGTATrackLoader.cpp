
#include <private/akPGTATrackLoader.h>
#include <private/akPGTATrack.h>
#include <private/akPGTASample.h>
#include <public/schema/track_generated.h>
#include <public/schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include <vector>
#include <memory>

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
    if (t == nullptr || t->samples() == nullptr || t->samples()->size() <= 0)
    {
        return nullptr;
    }

    auto* s = t->samples();
    int len = s->size();
    bool hasValidSamples = false;

    auto samples = std::make_unique<PGTATrackSample[]>(len);
    for (int i = 0; i < len; ++i)
    {
        PGTATrackSample &sample = samples[i];
        const auto* schemaSample = s->Get(i);
        if (!schemaSample)
        {
            continue;
        }

        const auto* name = schemaSample->name();
        const float probability = schemaSample->probability();
        const float volumeMultiplier = schemaSample->volumeMultiplier();

        if (!name || name->size() == 0 ||
            probability < 0.0f || volumeMultiplier < 0.0f)
        {
            continue;
        }

        sample.sampleName = name->c_str();
        sample.startTime = schemaSample->startTime();
        sample.frequency = schemaSample->frequency();
        sample.probability = probability;
        sample.volumeMultiplier = volumeMultiplier;

        bool hasInvalidGroup = false;
        const auto* groupIds = schemaSample->groupIds();
        if (!groupIds || groupIds->size() == 0)
        {
            continue;
        }

        const int numGroups = groupIds->size();
        sample.groups.resize(numGroups);
        for (int j = 0; j < numGroups; ++j)
        {
            const auto* group = groupIds->Get(j);
            const flatbuffers::Vector<int8_t>* uuid = nullptr;
            if (!group || !(uuid = group->uuid()) || uuid->size() != NUM_UUID_BYTES)
            {
                hasInvalidGroup = true;
                break;
            }
            memcpy(sample.groups[j].bytes, uuid->Data(), sizeof(sample.groups[j].bytes));
        }

        if (!hasInvalidGroup)
        {
            hasValidSamples = true;
        }
    }

    if (!hasValidSamples)
    {
        return nullptr;
    }

    track->SetSamples(samples);
    return track;
}
