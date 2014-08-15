
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
    
    int numSamples = m_engineTrack->getSamples().size();
    m_nextCheckCountdowns.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        m_nextCheckCountdowns[i] = m_engineTrack->getSamples()[i].GetStartTime();
    }

    int numGroups = (int)m_engineTrack->getGroups().size();
    for (int i = 0; i < numGroups; ++i)
    {
        m_groupRemainingDurations.emplace_back(TimeDuration::zero());
        m_readyGroupSamples.emplace_back(std::vector<uint16_t>());
    }
}

void SampleScheduler::Update(TimeDuration dt)
{
    using namespace std::chrono;

    for (auto groupEndTime = m_groupRemainingDurations.begin(); groupEndTime != m_groupRemainingDurations.end(); ++groupEndTime)
    { 
        if (dt <= *groupEndTime)
        {
            *groupEndTime -= dt;
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
        bool inGroup = sampleGroup != sampleGroups.end();
        if (shouldPlay && inGroup)
        {
            if (m_groupRemainingDurations[sampleGroup->second] > m_nextCheckCountdowns[i])
            {
                shouldPlay = false;
            }
            else
            {
                m_readyGroupSamples[sampleGroup->second].emplace_back(i);
            }
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
 
        if (shouldPlay && !inGroup)
        {
            m_streamBuffers[i]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());  
        } 
        m_nextCheckCountdowns[i] += std::max(engineSample.GetFrequency(), audioSample->getDuration());

        //std::cout << "msTime " << i << " : " << msTime << std::endl;
        //std::cout << "nextPlay " << i << " : " << m_nextPlayTime[i] << std::endl;
        //std::cout << "shouldPlay " << i << " : " << shouldPlay << std::endl;
    }

    int numGroups = (int)m_readyGroupSamples.size();
    for (int i = 0; i < numGroups; ++i)
    {
        int numReadySamples = (int)m_readyGroupSamples[i].size();

        if (numReadySamples == 0)
        {
            continue;
        }
        int sampleIdx = randomIntInRange(0, numReadySamples - 1);
        int sampleNum = m_readyGroupSamples[i][sampleIdx];
        const auto &engineSample = m_engineTrack->getSamples()[sampleNum];
        const auto *audioSample = engineSample.getSample();

        m_streamBuffers[sampleNum]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());
        m_groupRemainingDurations[i] = m_nextCheckCountdowns[sampleNum];
        m_readyGroupSamples[i].clear();
    }


    for (auto groupEndTime = m_groupRemainingDurations.begin(); groupEndTime != m_groupRemainingDurations.end(); ++groupEndTime)
    {
        if (dt > *groupEndTime)
        {
            *groupEndTime = TimeDuration::zero();
        }
    }
}
