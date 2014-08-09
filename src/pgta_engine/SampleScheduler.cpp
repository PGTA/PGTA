
#include "SampleScheduler.h"

#include "EngineTrack.h"
#include "EngineSample.h"
#include "AudioStreamBuffer.h"
#include "AudioSample.h"
#include "Probability.h"
#include <iostream>
#include <algorithm>
#include <SDL_timer.h>

SampleScheduler::SampleScheduler(EngineTrack *track,
                                 std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers):
    m_engineTrack(track),
    m_streamBuffers(std::move(streamBuffers)),
    m_nextPlayTime(track->getSamples().size(), 0)
{
}

void SampleScheduler::Update()
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

        std::cout << "msTime " << i << " : " << msTime << std::endl;
        std::cout << "nextPlay " << i << " : " << m_nextPlayTime[i] << std::endl;
        std::cout << "shouldPlay " << i << " : " << shouldPlay << std::endl;
    }
}
