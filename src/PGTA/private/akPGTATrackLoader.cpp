
#include <private/akPGTATrackLoader.h>
#include <private/akPGTATrack.h>
#include <private/akPGTATrackTypes.h>
#include <public/schema/track_generated.h>
#include <public/schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include <vector>
#include <map>

using Samples = std::vector<PGTATrackSample>;
using SchemaSamples = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Sample> >;
using Groups = std::vector<PGTATrackGroup>;
using SchemaGroups = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Group> >;
using GroupRestrictions = std::map<PGTAUUID, std::vector<PGTAUUID> >;
using SchemaRestritions = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Restriction> >;

static PGTATrack* LoadBinaryTrack(const uint8_t* src, const size_t length, PGTATrack* track);
static PGTATrack* LoadAsciiTrack(const char* src, const size_t length, PGTATrack* track);
static PGTATrack* InitTrackData(PGTATrack* const track, const PGTASchema::Track* t);
static uint16_t InitSamples(uint16_t numSamples, Samples* samples, const SchemaSamples* schemaSamples);
static uint16_t InitGroups(uint16_t numGroups, Groups* groups, const SchemaGroups* schemaGroups);
static uint16_t InitGroupRestrictions(uint16_t numRestrictions, GroupRestrictions* groupRestrictions, 
    const SchemaRestritions* schemaRestricitons);

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

uint16_t InitSamples(uint16_t numSamples, Samples* samples, const SchemaSamples* schemaSamples)
{
    uint16_t numValidSamples = 0;
    for (int i = 0; i < numSamples; ++i)
    {
        PGTATrackSample &sample = (*samples)[numValidSamples];
        const auto* schemaSample = schemaSamples->Get(i);

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

        size_t length = name->size();
        sample.sampleName = new char[length + 1];
        strcpy(sample.sampleName, name->c_str());

        const flatbuffers::String* defaultFile = schemaSample->defaultFile();
        if (!defaultFile || defaultFile->size() == 0)
        {
            sample.defaultFile = nullptr;
        }
        else
        {
            length = defaultFile->size();
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
            continue;
        }
        else
        {
            sample.group = new PGTAUUID();
            *sample.group = group->c_str();
        }

        sample.id = numValidSamples++;
    }

    return numValidSamples;
}

static uint16_t InitGroups(uint16_t numGroups, Groups* groups, const SchemaGroups* schemaGroups)
{
    uint16_t numValidGroups = 0;
    for (int i = 0; i < numGroups; ++i)
    {
        PGTATrackGroup &group = (*groups)[numValidGroups];
        auto schemaGroup = schemaGroups->Get(i);
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

    return numValidGroups;
}

static uint16_t InitGroupRestrictions(uint16_t numRestrictions, GroupRestrictions* groupRestrictions,
    const SchemaRestritions* schemaRestricitons)
{
    uint16_t numValidRestrictions = 0;
    for (int i = 0; i < numRestrictions; ++i)
    {
        PGTAUUID uuid1, uuid2;
        const flatbuffers::String* group1 = schemaRestricitons->Get(i)->group1();
        if (!group1 || group1->size() != PGTAUUID::UUID_NUM_BYTES)
        {
            continue;
        }

        const flatbuffers::String* group2 = schemaRestricitons->Get(i)->group2();
        if (!group2 || group2->size() != PGTAUUID::UUID_NUM_BYTES)
        {
            continue;
        }

        memcpy(uuid1.bytes, group1->Data(), sizeof(uuid1.bytes));
        memcpy(uuid2.bytes, group2->Data(), sizeof(uuid2.bytes));

        (*groupRestrictions)[uuid1].emplace_back(uuid2);
        (*groupRestrictions)[uuid2].emplace_back(uuid1);
        numValidRestrictions++;
    }

    return numValidRestrictions;
}

static PGTATrack* InitTrackData(PGTATrack* const track, const PGTASchema::Track* t)
{
    if (t == nullptr || t->samples() == nullptr || t->samples()->size() == 0)
    {
        return nullptr;
    }

    const SchemaSamples* s = t->samples();
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
    Samples samples(numSamples);
    numValidSamples = InitSamples(numSamples, &samples, s);
    samples.resize(numValidSamples);

    uint16_t numValidGroups = 0;
    Groups groups(numGroups);
    numValidGroups = InitGroups(numGroups, &groups, t->groups());
    groups.resize(numValidGroups);

    int numValidRestrictions = 0;
    GroupRestrictions groupRestrictions;
    numValidRestrictions = InitGroupRestrictions(numRestrictions, &groupRestrictions, t->restrictions());

    if (numValidSamples == 0)
    {
        return nullptr;
    }
    track->SetSamples(samples);
    track->SetGroups(groups);
    track->SetRestrictions(groupRestrictions, numValidRestrictions);
    return track;
}
