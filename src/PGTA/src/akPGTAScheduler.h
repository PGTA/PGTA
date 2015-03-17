
#pragma once

#include "akPGTATrackTypes.h"
#include "akPGTATrack.h"
#include "akPGTASchedulerRNG.h"
#include <akAudioMixer.h>
#include <string>
#include <vector>
#include <memory>
#include <utility>

struct MixRequest
{
    bool isPrimary;
    uint32_t delay;
    uint16_t sampleId;

    bool operator<(const MixRequest& other) const;
};

struct ScheduleData
{
    uint32_t samplesUntilPlay;
    int32_t samplesOffPeriod;
};

class PGTAScheduler
{
public:
    PGTAScheduler();
    ~PGTAScheduler();

    bool Initialize(const PGTAConfig& config);
    void SetPrimaryTrack(const PGTATrack* track);
    PGTABuffer Update(const float deltaSeconds);

private:
    PGTABuffer MixScheduleRequests(uint32_t deltaSamples, std::vector<MixRequest>& mixRequest);
    int32_t ConvertTimeToSamples(const float deltaTime) const;
    int32_t ConvertBeatsToSamples(const float deltaBeats) const;

    void SelectSchedulingCandidates(const bool isPrimary, const uint32_t deltaSamples);
    bool HasGroupConflict(const std::string& group, const uint32_t delay);
    bool HasRestrictionConflict(const bool isPrimary, const std::string& group, const uint32_t delay);

private:
    std::vector<std::pair<std::string, uint32_t>> m_groupsNextSchedule;
    std::vector<std::pair<std::string, uint32_t>> m_endingGroups;

    const PGTATrack* m_primaryTrack;
    float m_primaryWeight;
    std::vector<ScheduleData> m_primaryNextSchedules;

    const PGTATrack* m_transTrack;
    std::vector<ScheduleData> m_transNextSchedules;
    
    PGTASchedulerRNG m_rng;
    std::vector<std::pair<const std::string, MixRequest>> m_pooledRequests;
    std::vector<uint16_t> m_poolSchedulerOrder;
    std::vector<MixRequest> m_mixRequests;

    PGTAConfig m_config;

    akAudioMixer::AudioMixer* m_mixer;
    
    std::vector<int16_t> m_bufferData;
};
