
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
    m_nextCheckCountdowns.resize(track->getSamples().size());
}

void SampleScheduler::Update(TimeDuration dt)
{
    using namespace std::chrono;

    int numEngineSamples = (int)m_engineTrack->getSamples().size();
    for (int i = 0; i < numEngineSamples; ++i)
    {
        if (dt < m_nextCheckCountdowns[i])
        {
            m_nextCheckCountdowns[i] -= dt;
            continue;
        }

        const auto &engineSample = m_engineTrack->getSamples()[i];
        bool shouldPlay = true;// evalProbability(engineSample.GetProbability());
        if (!shouldPlay)
        {
            continue;
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

        const auto *audioSample = engineSample.getSample();
        m_streamBuffers[i]->PushSamples(audioSample->getSamples(), audioSample->getNumSamples());

        m_nextCheckCountdowns[i] += std::max(engineSample.GetFrequency(), audioSample->getDuration());

        //std::cout << "msTime " << i << " : " << msTime << std::endl;
        //std::cout << "nextPlay " << i << " : " << m_nextPlayTime[i] << std::endl;
        std::cout << "shouldPlay " << i << " : " << shouldPlay << std::endl;
    }
}
