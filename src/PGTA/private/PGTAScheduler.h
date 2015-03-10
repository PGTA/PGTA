#pragma once

#include <private/akPGTATrackTypes.h>
#include <private/akPGTATrack.h>
#include <akAudioMixer.h>
#include <string>
#include <vector>
#include <utility>

struct MixRequest
{
    // Which track
    uint16_t sampleId;
    uint32_t delay;
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
    
    std::vector<std::pair<std::string, uint32_t> > m_groupsNextShcedule;

    const PGTATrack* m_primaryTrack;
    float m_primaryWeight;
    std::vector<uint32_t> m_primaryNextShedules;

    const PGTATrack* m_transTrack;
    std::vector<uint32_t> m_transNextShedules;

    std::map < std::string, std::pair<PGTATrack*, uint16_t> > m_groupReadyPools;
    PGTAConfig m_config;

    akAudioMixer::AudioMixer* m_mixer;
    
    std::vector<int16_t> m_bufferData;

    PGTABuffer MixScheduleRequests(uint32_t deltaSamples, std::vector<MixRequest>& mixRequest);
    uint32_t ConvertTimeToSamples(float delta);
};