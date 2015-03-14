#pragma once

#include <private/akPGTATrackTypes.h>
#include <private/akPGTATrack.h>
#include <private/akPGTASchedulerRNG.h>
#include <akAudioMixer.h>
#include <string>
#include <vector>
#include <memory>
#include <utility>

struct MixRequest
{
    // TODO: Add track
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
    PGTABuffer Update(const float delta);
private:
    PGTABuffer MixScheduleRequests(uint32_t deltaSamples, std::vector<MixRequest>& mixRequest);
    int32_t ConvertTimeToSamples(const float delta);

    std::vector<std::pair<std::string, uint32_t>> m_groupsNextSchedule;
    std::vector<std::pair<std::string, uint32_t>> m_endingGroups;

    const PGTATrack* m_primaryTrack;
    float m_primaryWeight;
    std::vector<ScheduleData> m_primaryNextSchedules;

    const PGTATrack* m_transTrack;
    std::vector<ScheduleData> m_transNextSchedules;
    
    PGTASchedulerRNG m_rng;
    std::map<std::string, std::vector<MixRequest>> m_groupReadyPools;
    std::vector<MixRequest> m_mixRequests;

    PGTAConfig m_config;

    akAudioMixer::AudioMixer* m_mixer;
    
    std::vector<int16_t> m_bufferData;
};