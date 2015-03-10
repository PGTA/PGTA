#pragma once

#include <private/akPGTATrackTypes.h>
#include <private/akPGTATrack.h>
#include <akAudioMixer.h>
#include <string>
#include <vector>
#include <utility>

struct MixRequest
{
    // TODO: Add track
    uint32_t delay;
    uint16_t sampleId;

    bool operator<(const MixRequest& other) const;
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
    uint32_t ConvertTimeToSamples(float delta);

    std::vector<std::pair<std::string, uint32_t>> m_groupsNextShcedule;

    const PGTATrack* m_primaryTrack;
    float m_primaryWeight;
    std::vector<uint32_t> m_primaryNextSchedules;

    const PGTATrack* m_transTrack;
    std::vector<uint32_t> m_transNextSchedules;

    std::map <std::string, std::pair<PGTATrack*, uint16_t>> m_groupReadyPools;
    std::vector<MixRequest> m_mixRequests;
    PGTAConfig m_config;

    akAudioMixer::AudioMixer* m_mixer;
    
    std::vector<int16_t> m_bufferData;
};