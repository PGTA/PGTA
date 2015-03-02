
#include <private/akPGTATrackLoader.h>
#include <private/akPGTATrack.h>
#include <private/akPGTATrackTypes.h>
#include <public/schema/track_generated.h>
#include <public/schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include <vector>
#include <memory>
#include <utility>
#include <map>

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
    using SampleList = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Sample> >;

    if (t == nullptr || t->samples() == nullptr || t->samples()->size() == 0)
    {
        return nullptr;
    }

    bool hasValidSamples = false;
    const SampleList * s = t->samples();
    int numSamples = s->size();
    
    int numGroups = 0;
    if (t->groups() != nullptr)
    {
        numGroups = t->groups()->size();
    }

    int numRestrictions = 0;
    if (t->restrictions() != nullptr)
    {
        numRestrictions = t->restrictions()->size();
    }

    uint16_t numValidSamples = 0;
    std::vector<PGTATrackSample> samples(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        PGTATrackSample &sample = samples[numValidSamples];
        const auto* schemaSample = s->Get(i);

        if (!schemaSample)
        {
            continue;
        }

        const flatbuffers::String* name = schemaSample->name();
        const float probability = schemaSample->probability();
        const float volumeMultiplier = schemaSample->volumeMultiplier();

        if (!name || name->size() == 0 ||
            probability < 0.0f || volumeMultiplier < 0.0f)
        {
            continue;
        }

        size_t length = strlen(name->c_str());
        sample.sampleName = new char[length + 1];
        strcpy(sample.sampleName, name->c_str());

        const flatbuffers::String* defaultFile = schemaSample->defaultFile();
        if (!defaultFile || defaultFile->size() == 0)
        {
            sample.defaultFile = nullptr;
        }
        else
        {
            length = strlen(defaultFile->c_str());
            sample.defaultFile = new char[length + 1];
            strcpy(sample.defaultFile, defaultFile->c_str());
        }

        sample.startTime = schemaSample->startTime();
        sample.frequency = schemaSample->frequency();
        sample.probability = probability;
        sample.volumeMultiplier = volumeMultiplier;

        const flatbuffers::String* group = schemaSample->group();
        if (!group)
        {
            sample.group = nullptr;
            sample.id = numValidSamples++;
            hasValidSamples = true;
            continue;
        }
        else
        {
            sample.group = new PGTAUUID();
            memcpy(sample.group->bytes, group->c_str(), sizeof(sample.group->bytes));
        }

        hasValidSamples = true;
        sample.id = numValidSamples++;
    }

    uint16_t numValidGroups = 0;
    std::vector<PGTATrackGroup>groups(numGroups);
    for (int i = 0; i < numGroups; ++i)
    {
        PGTATrackGroup &group = groups[numValidGroups];
        auto schemaGroup = t->groups()->Get(i);
        if (schemaGroup->uuid() == nullptr || schemaGroup->uuid()->size() != PGTAUUID::UUID_NUM_BYTES)
        {
            continue;
        }

        memcpy(&group.uuid.bytes, schemaGroup->uuid()->Data(), sizeof(group.uuid.bytes));

        group.name = nullptr;
        if (schemaGroup->name() != nullptr && schemaGroup->name()->size() > 0)
        {
            group.name = new char[schemaGroup->name()->size() + 1];
            strcpy(group.name, schemaGroup->name()->c_str());
        }
        numValidGroups++;
    }

    int numValidRestrictions = 0;
    std::map<PGTAUUID, std::vector<PGTAUUID> > groupRestrictions;
    for (int i = 0; i < numRestrictions; ++i)
    {
        PGTAUUID uuid1, uuid2;
        const flatbuffers::String* group1 = t->restrictions()->Get(i)->group1();
        if (!group1 || group1->size() != PGTAUUID::UUID_NUM_BYTES)
        {
            continue;
        }

        const flatbuffers::String* group2  = t->restrictions()->Get(i)->group2();
        if (!group2 || group2->size() != PGTAUUID::UUID_NUM_BYTES)
        {
            continue;
        }

        memcpy(uuid1.bytes, group1->Data(), sizeof(uuid1.bytes));
        memcpy(uuid2.bytes, group2->Data(), sizeof(uuid2.bytes));

        groupRestrictions[uuid1].emplace_back(uuid2);
        groupRestrictions[uuid2].emplace_back(uuid1);
        numValidRestrictions++;
    }

    if (!hasValidSamples)
    {
        return nullptr;
    }
    track->SetSamples(samples, numValidSamples);
    track->SetGroups(groups, numValidGroups);
    track->SetRestrictions(groupRestrictions, numValidRestrictions);
    return track;
}
