
#pragma once

#include "IPGTA.h"
#include "AudioMixer.h"
#include "SampleScheduler.h"
#include <vector>
#include <memory>

class EngineTrack;
class AudioStreamBuffer;

class PGTA: public IPGTA
{
public:
    virtual void Initialize(const PGTAConfig &config) override;
    
    virtual void StartPlayback(const std::string &trackName) override;
    
    virtual void StopPlayback() override;
    
    virtual void TransitionEvent(const std::string &event, uint8_t transitionAmount) override;
    
    virtual void Update() override;
    
    virtual int GetAudioData(char *buffer, int length) override;

    virtual const char* GetAudioBuffer(int& numSamples) override;

private:
    std::unique_ptr<EngineTrack> m_currentTrack;
    std::vector<std::unique_ptr<AudioStreamBuffer>> m_streamBuffers;
    AudioMixer<16> m_mixer;
    SampleScheduler m_scheduler;
    std::vector<char> m_outputBuffers[2];
    PGTAConfig m_config;
};
