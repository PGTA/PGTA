
#include <private/akPGTATrack.h>
#include <public/akPGTATypes.h>
#include <algorithm>

PGTATrack::PGTATrack()
{
    m_dataReferences = 0;
}

PGTATrack::~PGTATrack()
{
}

void PGTATrack::SetSamples(std::vector<PGTATrackSample>& samples, uint16_t numSamples)
{
    m_samples.resize(numSamples);
    std::move(samples.begin(), samples.begin() + (numSamples), m_samples.begin());
}

void PGTATrack::SetGroups(std::vector<PGTATrackGroup>& groups, uint16_t numGroups)
{
    m_groups.resize(numGroups);
    std::move(groups.begin(), groups.begin() + (numGroups), m_groups.begin());
}

void PGTATrack::SetRestrictions(std::map<PGTAUUID, std::vector<PGTAUUID> >& restrictions, 
    const uint16_t numRestrictions)
{
    m_numRestrictions = numRestrictions;
    m_groupRestrictions = std::move(restrictions);

}

PGTARestrictionData* PGTATrack::CopyRestrictionData() const
{
    PGTARestrictionData* copyRestrictions = new PGTARestrictionData[m_numRestrictions];

    uint16_t numCopied = 0;
    for (auto &i : m_groupRestrictions)
    {
        for (auto &j : i.second)
        {
            PGTARestrictionData& copyRestriction = copyRestrictions[numCopied];
            bool alreadyCopied = false;
            for (int k = numCopied - 1; k >= 0; --k)
            {
                char* copyUUID1 = copyRestrictions[k].group1UUID;
                char* copyUUID2 = copyRestrictions[k].group2UUID;
                if ((i.first == copyUUID1 && j == copyUUID2) ||
                    (i.first == copyUUID2 && j == copyUUID1))
                {
                    alreadyCopied = true;
                    break;
                }
            }

            if (!alreadyCopied)
            {
                copyRestriction.group1UUID = new char[PGTAUUID::UUID_NUM_BYTES + 1];
                copyRestriction.group2UUID = new char[PGTAUUID::UUID_NUM_BYTES + 1];
                memcpy(copyRestriction.group1UUID, i.first.bytes, PGTAUUID::UUID_NUM_BYTES);
                memcpy(copyRestriction.group2UUID, j.bytes, PGTAUUID::UUID_NUM_BYTES);
                copyRestriction.group1UUID[PGTAUUID::UUID_NUM_BYTES] = 0;
                copyRestriction.group2UUID[PGTAUUID::UUID_NUM_BYTES] = 0;

                numCopied++;
            }
        }
    }

    return copyRestrictions;
}

PGTAGroupData* PGTATrack::CopyGroupData() const
{
    int numGroups = GetNumGroups();
    PGTAGroupData* copyGroups = new PGTAGroupData[numGroups];
    for (int i = 0; i < numGroups; ++i)
    {
        PGTAGroupData& group = copyGroups[i];
        group.name = m_groups[i].name;
        group.uuid = new char[PGTAUUID::UUID_NUM_BYTES + 1];
        group.uuid[PGTAUUID::UUID_NUM_BYTES] = 0;
        memcpy(group.uuid, m_groups[i].uuid.bytes, PGTAUUID::UUID_NUM_BYTES);
    }

    return copyGroups;
}

PGTASampleData* PGTATrack::CopySampleData() const
{
    int numSamples = GetNumSamples();
    PGTASampleData* copySamples = new PGTASampleData[numSamples];
    for (int i = 0; i < numSamples; ++i)
    {
        PGTASampleData& sample = copySamples[i];
        const PGTATrackSample &sourceSample = m_samples[i];

        sample.frequency = sourceSample.frequency;
        sample.probability = sourceSample.probability;
        sample.startTime = sourceSample.startTime;
        sample.volumeMultiplier = sourceSample.volumeMultiplier;

        sample.sampleName = nullptr;
        if (sourceSample.sampleName != nullptr)
        {
            size_t length = strlen(sourceSample.sampleName);
            sample.sampleName = new char[length + 1];
            strcpy(sample.sampleName, sourceSample.sampleName);
        }

        sample.defaultFile = nullptr;
        if (sourceSample.defaultFile != nullptr)
        {
            size_t length = strlen(sourceSample.defaultFile);
            sample.defaultFile = new char[length + 1];
            strcpy(sample.defaultFile, sourceSample.defaultFile);
        }

        sample.groupUUID = nullptr;
        if (sourceSample.group != nullptr)
        {
            sample.groupUUID = new char[PGTAUUID::UUID_NUM_BYTES + 1];
            memcpy(sample.groupUUID, sourceSample.group->bytes, sizeof(char) * PGTAUUID::UUID_NUM_BYTES);
            sample.groupUUID[PGTAUUID::UUID_NUM_BYTES] = 0;        
        }
    }
    return copySamples;
}

PGTATrackData* PGTATrack::GetTrackData(HPGTATrack trackHandle)
{
    if (m_dataReferences > 0)
    {
        m_dataReferences++;
        return m_trackData;
    }

    m_trackData = new PGTATrackData();
    m_trackData->samples = CopySampleData();
    m_trackData->numSamples = GetNumSamples();

    if (m_trackData->samples == nullptr)
    {
        delete m_trackData;
        return nullptr;
    }

    m_trackData->trackHandle = trackHandle;
    m_trackData->groups = CopyGroupData();
    m_trackData->numGroups = GetNumGroups();

    m_trackData->numRestrictions = GetNumRestrictions();
    m_trackData->restrictions = CopyRestrictionData();

    m_dataReferences++;
    return m_trackData;
}

void PGTATrack::FreeTrackData()
{
    if (m_dataReferences <= 0)
    {
        m_dataReferences = 0;
        return;
    }
    m_dataReferences--;

    if (m_dataReferences != 0)
    {
        return;
    }

    int numSamples = m_trackData->numSamples;
    for (int i = 0; i < numSamples; ++i)
    {
        PGTASampleData &sample = m_trackData->samples[i];
        delete sample.sampleName;
        delete sample.defaultFile;
        delete sample.groupUUID;
    }
    delete m_trackData->samples;

    int numGroups = m_trackData->numGroups;
    for (int i = 0; i < numGroups; ++i)
    {
        PGTAGroupData& group = m_trackData->groups[i];
        delete group.name;
        delete group.uuid;
    }
    delete m_trackData->groups;

    int numRestrictions = m_trackData->numRestrictions;
    for (int i = 0; i < numRestrictions; ++i)
    {
        PGTARestrictionData& restriction = m_trackData->restrictions[i];
        delete restriction.group1UUID;
        delete restriction.group2UUID;
    }
    delete m_trackData->restrictions;

    delete m_trackData;
    m_trackData = nullptr;
}