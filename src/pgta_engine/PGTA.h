
#pragma once

#include "IPGTA.h"
#include "AudioMixer.h"
#include "SampleScheduler.h"
#include <vector>
#include <queue>
#include <memory>
#include <chrono>

class EngineTrack;
class AudioStreamBuffer;

class PGTAEngine: public PGTA::IPGTA
{
    using TimeDuration = std::chrono::high_resolution_clock::duration;
public:
    PGTAEngine();
    ~PGTAEngine();

    virtual void Initialize(const PGTA::PGTAConfig &config) override;

    virtual PGTA::PGTATrackHandle const LoadTrack(const std::string &trackName) override;

    virtual void FreeTrack(const PGTA::PGTATrackHandle  instance) override;

    virtual bool PlayTrack(const PGTA::PGTATrackHandle  instance) override;

    virtual void StartPlayback() override;

    virtual void StopPlayback() override;

    virtual void TransitionEvent(const std::string &event, uint8_t transitionAmount) override;

    virtual const PGTA::OutputBuffer* Update(int& numOutputBuffers) override;

    virtual const PGTA::OutputBuffer* GetOutputBuffers(int& numOutputBuffers) const override;

private:
    TimeDuration NumSamplesToDuration(int numSamples);

private:
    std::chrono::high_resolution_clock::time_point m_mixTime;
    std::vector<EngineTrack*> m_tracks;
    AudioMixer<16> m_mixer;
    SampleScheduler m_scheduler;

    std::queue<int> m_freeBuffers;
    std::vector<std::unique_ptr<char[]>> m_mixBuffers;
    std::vector<PGTA::OutputBuffer> m_outputBuffers;

    bool m_isPlaying;
    PGTA::PGTAConfig m_config;
};
