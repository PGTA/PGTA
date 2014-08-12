
#pragma once

#include "IPGTA.h"
#include "AudioMixer.h"
#include "SampleScheduler.h"
#include <vector>
#include <memory>
#include <chrono>

class EngineTrack;
class AudioStreamBuffer;

class PGTAEngine: public PGTA::IPGTA
{
public:
    PGTAEngine();
    ~PGTAEngine();

    virtual void Initialize(const PGTA::PGTAConfig &config) override;

    virtual bool StartPlayback(const std::string &trackName) override;

    virtual void StopPlayback() override;

    virtual void TransitionEvent(const std::string &event, uint8_t transitionAmount) override;

    virtual void Update() override;

    virtual const PGTA::OutputBuffer* GetOutputBuffers(int& numBuffers) const override;

private:
    std::chrono::high_resolution_clock::time_point m_lastUpdate;
    std::unique_ptr<EngineTrack> m_currentTrack;
    AudioMixer<16> m_mixer;
    SampleScheduler m_scheduler;
    std::vector<char> m_mixerOutput;
    std::vector<PGTA::OutputBuffer> m_outputBuffers;
    PGTA::PGTAConfig m_config;
};
