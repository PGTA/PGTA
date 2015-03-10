
#include <private/akPGTATrackLoader.h>
#include <private/akPGTATrack.h>
#include <private/akPGTATrackTypes.h>
#include <public/schema/track_generated.h>
#include <public/schema/track.fbs.h>
#include <flatbuffers/idl.h>
#include <vector>
#include <map>

using std::vector;

using SchemaSamples = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Sample> >;
using SchemaGroups = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Group> >;
using GroupRestrictions = std::map<std::string, std::vector<std::string> >;
using SchemaRestritions = flatbuffers::Vector<flatbuffers::Offset<PGTASchema::Restriction> >;

static PGTATrack* LoadBinaryTrack(const uint8_t* src, const size_t length, PGTATrack* track);
static PGTATrack* LoadAsciiTrack(const char* src, PGTATrack* track);
static PGTATrack* InitTrackData(PGTATrack* const track, const PGTASchema::Track* schemaTrack);

static void InitSamples(const SchemaSamples& schemaSamples, vector<PGTATrackSample>* samples);
static void InitGroups(const SchemaGroups& schemaGroups, vector<PGTATrackGroup>* groups);
static uint16_t InitGroupRestrictions(const SchemaRestritions& schemaRestricitons,
                                      GroupRestrictions* groupRestrictions);

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
        return LoadAsciiTrack(src, track);
    }
}

static PGTATrack* LoadBinaryTrack(const uint8_t* src, const size_t length, PGTATrack* track)
{
    flatbuffers::Verifier verifier(src, length);
    if (!PGTASchema::VerifyTrackBuffer(verifier))
    {
        return nullptr;
    }

    const PGTASchema::Track* schemaTrack = PGTASchema::GetTrack(src);
    if (!schemaTrack)
    {
        return nullptr;
    }

    return InitTrackData(track, schemaTrack);
}

static PGTATrack* LoadAsciiTrack(const char* src, PGTATrack* track)
{
    flatbuffers::Parser parser;
    if (!parser.Parse(PGTASchemaHeader::TRACK_FBS) || !parser.Parse(src))
    {
        return nullptr;
    }

    const PGTASchema::Track* schemaTrack = PGTASchema::GetTrack(parser.builder_.GetBufferPointer());
    if (!schemaTrack)
    {
        return nullptr;
    }
 
    return InitTrackData(track, schemaTrack);
}

static void InitSamples(const SchemaSamples& schemaSamples, vector<PGTATrackSample>* samples)
{
    assert(samples);
    const int numSamples = schemaSamples.size();
    int16_t sampleId = 0;
    samples->reserve(numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        const PGTASchema::Sample* schemaSample = schemaSamples.Get(i);
        if (!schemaSample)
        {
            continue;
        }

        const flatbuffers::String* name = schemaSample->name();
        const flatbuffers::String* defaultFile = schemaSample->defaultFile();
        const flatbuffers::String* group = schemaSample->group();
        const int64_t startTime = schemaSample->startTime();
        const int64_t frequency = schemaSample->frequency();
        const float probability = schemaSample->probability();
        const float volumeMultiplier = schemaSample->volumeMultiplier();

        // TODO: sanity checks for schemaSample properties
        if (!name || (name->size() == 0))
        {
            continue;
        }

        PGTATrackSample sample{};
        sample.sampleName.assign(name->c_str());
        if (defaultFile && defaultFile->size() > 0)
        {
            sample.defaultFile.assign(defaultFile->c_str());
        }
        if (group && group->size() > 0)
        {
            sample.group.assign(group->c_str());
        }
        sample.startTime = startTime;
        sample.frequency = frequency;
        sample.probability = probability;
        sample.volumeMultiplier = volumeMultiplier;
        sample.id = sampleId++;

        samples->emplace_back(std::move(sample));
    }
    samples->shrink_to_fit();
}

static void InitGroups(const SchemaGroups& schemaGroups, vector<PGTATrackGroup>* groups)
{
    assert(groups);
    const int numGroups = schemaGroups.size();
    groups->reserve(numGroups);

    for (int i = 0; i < numGroups; ++i)
    {
        const PGTASchema::Group* schemaGroup = schemaGroups.Get(i);
        if (!schemaGroup)
        {
            continue;
        }

        const flatbuffers::String* uuid = schemaGroup->uuid();
        const flatbuffers::String* name = schemaGroup->name();
        if (!uuid || uuid->size() != PGTAUUID::UUID_NUM_BYTES ||
            !name || name->size() == 0)
        {
            // TODO: allow nameless groups?
            continue;
        }

        PGTATrackGroup group{};
        group.name.assign(name->c_str());
        group.uuid.assign(uuid->c_str());

        groups->emplace_back(std::move(group));
    }
    groups->shrink_to_fit();
}

static uint16_t InitGroupRestrictions(const SchemaRestritions& schemaRestricitons,
                                      GroupRestrictions* groupRestrictions)
{
    assert(groupRestrictions);
    uint16_t numValidRestrictions = 0;

    const int numRestrictions = schemaRestricitons.size();
    for (int i = 0; i < numRestrictions; ++i)
    {
        const PGTASchema::Restriction* schemaRestriction = schemaRestricitons.Get(i);
        if (!schemaRestriction)
        {
            continue;
        }

        const flatbuffers::String* group1 = schemaRestriction->group1();
        const flatbuffers::String* group2 = schemaRestriction->group2();
        if (!group1 || group1->size() != PGTAUUID::UUID_NUM_BYTES ||
            !group2 || group2->size() != PGTAUUID::UUID_NUM_BYTES)
        {
            continue;
        }

        std::string uuid1(group1->c_str());
        std::string uuid2(group2->c_str());

        auto& groupList1 = (*groupRestrictions)[uuid1];
        auto& groupList2 = (*groupRestrictions)[uuid2];
        groupList1.emplace_back(std::move(uuid2));
        groupList2.emplace_back(std::move(uuid1));

        numValidRestrictions += 1;
    }

    return numValidRestrictions;
}

static PGTATrack* InitTrackData(PGTATrack* const track, const PGTASchema::Track* schemaTrack)
{
    const SchemaSamples* schemaSamples = schemaTrack->samples();
    const SchemaGroups* schemaGroups = schemaTrack->groups();
    const SchemaRestritions* schemaRestrictions = schemaTrack->restrictions();
    if (!schemaSamples || schemaSamples->size() == 0)
    {
        return nullptr;
    }

    vector<PGTATrackSample> samples;
    InitSamples(*schemaSamples, &samples);
    if (samples.empty())
    {
        return nullptr;
    }
    track->SetSamples(samples);

    vector<PGTATrackGroup> groups;
    InitGroups(*schemaGroups, &groups);
    track->SetGroups(groups);

    GroupRestrictions groupRestrictions;
    const uint16_t numValidRestrictions = InitGroupRestrictions(*schemaRestrictions,
                                                                &groupRestrictions);
    track->SetRestrictions(groupRestrictions, numValidRestrictions);
    return track;
}
