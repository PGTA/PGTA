
#include <private/akPGTATrack.h>
#include <public/akPGTATypes.h>
#include <string.h>

PGTATrack::PGTATrack() : 
    m_dataReferences(0)
{
}

PGTATrack::~PGTATrack()
{
}

void PGTATrack::SetSamples(std::vector<PGTATrackSample>& samples)
{
    m_samples = std::move(samples);
}

void PGTATrack::SetGroups(std::vector<PGTATrackGroup>& groups)
{
    m_groups = std::move(groups);
}

void PGTATrack::SetRestrictions(std::map<std::string, std::vector<std::string> >& restrictions, 
    const uint16_t numRestrictions)
{
    m_numRestrictions = numRestrictions;
    m_groupRestrictions = std::move(restrictions);
}

std::vector<PGTARestrictionData> PGTATrack::CopyRestrictionData() const
{
    int numRestrictions = GetNumRestrictions();
    std::vector<PGTARestrictionData> copyRestrictions;

    if (!m_trackData)
    {
        return copyRestrictions;
    }

    copyRestrictions.resize(numRestrictions);
    m_trackData->cachedRestrictions.resize(numRestrictions);

    uint16_t numCached = 0;
    for (auto &i : m_groupRestrictions)
    {
        for (auto &j : i.second)
        {
            PGTACachedRestrictionData& cachedRestriction = m_trackData->cachedRestrictions[numCached];
            
            // Is true if this restriction was already found in another group
            bool alreadyCopied = false;
            for (int k = numCached - 1; k >= 0; --k)
            {
                std::string UUID1 = m_trackData->cachedRestrictions[k].group1UUID;
                std::string UUID2 = m_trackData->cachedRestrictions[k].group2UUID;
                if ((i.first == UUID1 && j == UUID2) ||
                    (i.first == UUID2 && j == UUID1))
                {
                    alreadyCopied = true;
                    break;
                }
            }

            if (!alreadyCopied)
            {
                cachedRestriction.group1UUID = i.first.c_str();
                cachedRestriction.group2UUID = j.c_str();

                copyRestrictions[numCached].group1UUID = cachedRestriction.group1UUID.data();
                copyRestrictions[numCached].group2UUID = cachedRestriction.group2UUID.data();

                numCached++;

                if (numCached == numRestrictions)
                {
                    return copyRestrictions;
                }
            }
        }
    }

    return copyRestrictions;
}

std::vector<PGTAGroupData> PGTATrack::CopyGroupData() const
{
    int numGroups = GetNumGroups();
    std::vector<PGTAGroupData> copyGroups;

    if (!m_trackData)
    {
        return copyGroups;
    }

    copyGroups.resize(numGroups);
    m_trackData->cachedGroups.resize(numGroups);

    for (int i = 0; i < numGroups; ++i)
    {
        PGTACachedGroupData& cachedGroup = m_trackData->cachedGroups[i];
        PGTAGroupData& group = copyGroups[i];

        cachedGroup.name = m_groups[i].name.c_str();
        cachedGroup.uuid = m_groups[i].uuid.c_str();
        
        group.name = cachedGroup.name.c_str();
        group.uuid = cachedGroup.uuid.c_str();
    }

    return copyGroups;
}

std::vector<PGTASampleData> PGTATrack::CopySampleData() const
{
    int numSamples = GetNumSamples();
    std::vector<PGTASampleData> copySamples;

    if (!m_trackData)
    {
        return copySamples;
    }

    copySamples.resize(numSamples);
    m_trackData->cachedSamples.resize(numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        PGTACachedSampleData& cachedsample = m_trackData->cachedSamples[i];
        PGTASampleData& sample = copySamples[i];
        const PGTATrackSample &sourceSample = m_samples[i];

        cachedsample.sampleName = sourceSample.sampleName.c_str();
        cachedsample.defaultFile = sourceSample.defaultFile.c_str();
        cachedsample.groupUUID = sourceSample.group.c_str();

        sample.frequency = sourceSample.frequency;
        sample.probability = sourceSample.probability;
        sample.startTime = sourceSample.startTime;
        sample.volumeMultiplier = sourceSample.volumeMultiplier;
        sample.sampleName = cachedsample.sampleName.c_str();
        sample.defaultFile = cachedsample.defaultFile.c_str();
        sample.groupUUID = cachedsample.groupUUID.c_str();
    }

    return copySamples;
}

PGTATrackData PGTATrack::GetTrackData(HPGTATrack trackHandle)
{
    PGTATrackData trackData;
    if (m_dataReferences == 0)
    {      
        m_trackData = std::make_unique<PGTACachedTrackData>();
        
        m_trackData->samples = std::move(CopySampleData());
        m_trackData->groups = std::move(CopyGroupData());
        m_trackData->restrictions = std::move(CopyRestrictionData());

        m_trackData->trackHandle = trackHandle;
    }
    
    m_dataReferences++;

    trackData.numSamples = m_trackData->samples.size();
    trackData.samples = m_trackData->samples.data();

    trackData.numGroups = m_trackData->groups.size();
    trackData.groups = m_trackData->groups.data();

    trackData.numRestrictions = m_trackData->restrictions.size();
    trackData.restrictions = m_trackData->restrictions.data();

    trackData.trackHandle = m_trackData->trackHandle;
    return trackData;
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
}
