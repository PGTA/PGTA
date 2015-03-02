
#pragma once

#include <private/akPGTATrackTypes.h>
#include <public/akPGTATypes.h>
#include <memory>
#include <vector>
#include <map>

struct PGTATrackSample;

class PGTATrack
{
public:
    PGTATrack();
    ~PGTATrack();

    void SetSamples(std::vector<PGTATrackSample>& samples, const uint16_t numSamples);
    void SetGroups(std::vector<PGTATrackGroup>& groups, const uint16_t numGroups);
    void SetRestrictions(std::map<PGTAUUID, std::vector<PGTAUUID> >& restrictions, const uint16_t numRestrictions);
    uint16_t GetNumSamples() const { return m_samples.size(); }
    uint16_t GetNumGroups() const { return m_groups.size(); }
    uint16_t GetNumRestrictions() const { return m_numRestrictions; }
    PGTATrackData* GetTrackData(const HPGTATrack trackHandle);
    void FreeTrackData();
    PGTASampleData* CopySampleData() const;
    PGTAGroupData* CopyGroupData() const;
    PGTARestrictionData* CopyRestrictionData() const;

private:
    std::vector<PGTATrackSample> m_samples;
    std::vector<PGTATrackGroup> m_groups;
    std::map<PGTAUUID, std::vector<PGTAUUID> > m_groupRestrictions;
    uint16_t m_numRestrictions;

    int m_dataReferences;
    PGTATrackData* m_trackData;
};
