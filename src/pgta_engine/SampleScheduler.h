
#pragma once

#include "EngineSample.h"
#include "Probability.h"
#include "EngineGroup.h"
#include "EngineTrack.h"
#include "AudioStreamBuffer.h"
#include "AudioMixer.h"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <SDL_timer.h>

class SampleScheduler
{
    
public:
    SampleScheduler( EngineTrack *track, std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers ) :
    m_engineTrack(track),
    m_streamBuffers(std::move(streamBuffers)),
    m_nextPlayTime(track->getSamples().size(), 0)
    {
        
    }
    
    void Update()
    {
        uint32_t msTime = SDL_GetTicks();
        
        int numEngineSamples = (int)m_engineTrack->getSamples().size();
        
        for (int i = 0; i < numEngineSamples; ++i)
        {
            
            if (msTime < m_nextPlayTime[i])
            {
                continue;
            }
            const auto &engineSample = m_engineTrack->getSamples()[i];
            bool shouldPlay = evalProbability(engineSample.GetProbability());
            const auto *audioSample = engineSample.getSample();
            if (shouldPlay)
            {
                m_streamBuffers[i]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());
            }
            m_nextPlayTime[i] = msTime + std::max(engineSample.GetFrequency(), audioSample->getDuration());
            
            std::cout << "msTime : " << msTime << std::endl;
            std::cout << "nextPlay : " << m_nextPlayTime[i] << std::endl;
            std::cout << "shouldPlay : " << shouldPlay << std::endl;
        }
    }
private:
    bool IsGroupPlaying()
    {
        return true;
    }
private:
    EngineTrack *m_engineTrack;
    std::vector<std::unique_ptr<AudioStreamBuffer>> m_streamBuffers;
    std::vector<uint32_t> m_nextPlayTime;
    
};
