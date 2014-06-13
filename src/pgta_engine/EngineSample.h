
#pragma once

#include "AudioSample.h"

class EngineSample
{

public:
    EngineSample( AudioSample &sample ) :
        m_sample(sample)
    {
    }
    const AudioSample& getSample() const
    {
        return m_sample;
    }
private:
    AudioSample m_sample;
    // TODO list of audio processors to apply
};

