
#include "SampleScheduler.h"

#include "EngineTrack.h"
#include "EngineSample.h"
#include "AudioStreamBuffer.h"
#include "AudioSample.h"
#include "Probability.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <SDL_timer.h>

void SampleScheduler::Initialize(EngineTrack *track,
                                 std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers)
{
    m_engineTrack = track;
    m_streamBuffers = std::move(streamBuffers);
    m_nextCheckCountdowns.resize(track->getSamples().size());

    auto numGroups = (int)track->getGroups().size();
    for (int i = 0; i < numGroups; ++i)
    {
       // m_scheduledGroups[i] = std::chrono::duration::zero();
    }
}

void SampleScheduler::Update(TimeDuration dt)
{
    using namespace std::chrono;

    for (auto groupEndTime = m_scheduledGroups.begin(); groupEndTime != m_scheduledGroups.end(); ++groupEndTime)
    { 
        if (dt <= groupEndTime->second)
        {
            groupEndTime->second -= dt;
        }
    }

    int numEngineSamples = (int)m_engineTrack->getSamples().size();
    for (int i = 0; i < numEngineSamples; ++i)
    {
        if (dt <= m_nextCheckCountdowns[i])
        {
            m_nextCheckCountdowns[i] -= dt;
            continue;
        }

        const auto &engineSample = m_engineTrack->getSamples()[i];
        const auto *audioSample = engineSample.getSample();
        bool shouldPlay = evalProbability(engineSample.GetProbability()); 

        auto &sampleGroups = m_engineTrack->getSampleGroups();
        auto sampleGroup = sampleGroups.find(i);
        if (shouldPlay && sampleGroup != sampleGroups.end() && m_scheduledGroups[sampleGroup->second] > m_nextCheckCountdowns[i])
        {
            shouldPlay = false;
        }

        // padding code
        /*using SamplesDuration = duration<int, std::ratio<1, 44100>>;
        auto numPaddingSamples = duration_cast<SamplesDuration>(m_nextCheckCountdowns[i]).count();
        auto seconds = duration_cast<duration<double>>(m_nextCheckCountdowns[i]).count();
        auto samples = seconds * 44100;
        numPaddingSamples -= m_streamBuffers[i]->GetNumSamples();
        if (numPaddingSamples > 0)
        {
            m_paddingBuffer.resize(numPaddingSamples, 0);
            //std::fill(m_paddingBuffer.begin(), m_paddingBuffer.end(), 0);
            m_streamBuffers[i]->PushSamples(m_paddingBuffer.data(), numPaddingSamples);
        }*/
 
        if (shouldPlay)
        {
            m_streamBuffers[i]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());
            m_scheduledGroups[sampleGroup->second] = m_nextCheckCountdowns[i] + audioSample->getDuration();
        }
        m_nextCheckCountdowns[i] = std::max(engineSample.GetFrequency(), audioSample->getDuration());

        //std::cout << "msTime " << i << " : " << msTime << std::endl;
        //std::cout << "nextPlay " << i << " : " << m_nextPlayTime[i] << std::endl;
        std::cout << "shouldPlay " << i << " : " << shouldPlay << std::endl;
    }

    for (auto groupEndTime = m_scheduledGroups.begin(); groupEndTime != m_scheduledGroups.end(); ++groupEndTime)
    {
        if (dt > groupEndTime->second)
        {
            groupEndTime->second -= dt; //JORDAN MAKE THIS TIME DURATION 0
        }
    }
}
