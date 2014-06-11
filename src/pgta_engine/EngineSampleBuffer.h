
#pragma once

#include "EngineSample.h"
#include <cstdint>
#include <algorithm>

class EngineSampleBuffer
{
public:
    EngineSampleBuffer( EngineSample *sample ):
        m_sample(sample)
    {
        auto &audioSample = sample->getSample();
        m_currentPosition = audioSample.getSamples();
        m_endPosition = m_currentPosition + audioSample.getNumSamples();
    }
    uint32_t getAudioSamples( int16_t **out, uint32_t maxOut )
    {
        auto samplesLeft = static_cast<uint32_t>(m_endPosition - m_currentPosition);
        auto samplesGiven = std::min(samplesLeft, maxOut);
        out = m_currentPosition;
        m_currentPosition += samplesGiven;
        return samplesGiven;
    }
private:
    EngineSample *m_sample;
    int16_t *m_currentPosition;
    int16_t *m_endPosition;
};

