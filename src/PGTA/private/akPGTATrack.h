
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

    void SetSamples(std::vector<PGTATrackSample>& samples);
    void SetGroups(std::vector<PGTATrackGroup>& groups);
    void SetRestrictions(std::map<std::string, std::vector<std::string> >& restrictions, const uint16_t numRestrictions);
    
    uint16_t GetNumSamples() const { return static_cast<uint16_t>(m_samples.size()); }
    uint16_t GetNumGroups() const { return static_cast<uint16_t>(m_groups.size()); }
    uint16_t GetNumRestrictions() const { return m_numRestrictions; }

    const std::vector<PGTATrackSample>* GetSamples() const { return &m_samples; }
    
    PGTATrackData GetTrackData(const HPGTATrack trackHandle);
    void FreeTrackData();
    
    std::vector<PGTASampleData> CopySampleData() const;
    std::vector<PGTAGroupData> CopyGroupData() const;
    std::vector<PGTARestrictionData> CopyRestrictionData() const;

private:
    std::vector<PGTATrackSample> m_samples;
    std::vector<PGTATrackGroup> m_groups;
    std::map<std::string, std::vector<std::string> > m_groupRestrictions;
    uint16_t m_numRestrictions;

    int m_dataReferences;
    std::unique_ptr<PGTACachedTrackData> m_trackData;
};
