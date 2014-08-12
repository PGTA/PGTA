
#include "SampleScheduler.h"

#include "EngineTrack.h"
#include "EngineSample.h"
#include "AudioStreamBuffer.h"
#include "AudioSample.h"
#include "Probability.h"
#include <iostream>
#include <algorithm>
#include <SDL_timer.h>

void SampleScheduler::Initialize(EngineTrack *track,
                                 std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers)
{
    m_engineTrack = track;
    m_streamBuffers = std::move(streamBuffers);
    m_nextPlayTimes.resize(track->getSamples().size(),
                           std::chrono::high_resolution_clock::now());
}

void SampleScheduler::Update(TimePoint curTime)
{
    using namespace std::chrono;
    int numEngineSamples = (int)m_engineTrack->getSamples().size();

    for (int i = 0; i < numEngineSamples; ++i)
    {
        if (curTime < m_nextPlayTimes[i])
        {
            continue;
        }
        const auto &engineSample = m_engineTrack->getSamples()[i];
        bool shouldPlay = true;// evalProbability(engineSample.GetProbability());
        const auto *audioSample = engineSample.getSample();
        if (shouldPlay)
        {
            m_streamBuffers[i]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());
        }
        auto nextPlayDelta = std::max(engineSample.GetFrequency(), audioSample->getDuration());
        m_nextPlayTimes[i] = curTime + nextPlayDelta;

        //std::cout << "msTime " << i << " : " << msTime << std::endl;
        //std::cout << "nextPlay " << i << " : " << m_nextPlayTime[i] << std::endl;
        std::cout << "shouldPlay " << i << " : " << shouldPlay << std::endl;
    }
}
