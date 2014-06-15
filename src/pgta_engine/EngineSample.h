
#pragma once

#include "AudioSample.h"

class EngineSample
{
public:
	struct SampleProps
	{
		// time difference from start of samples
		// to next potential playback 
		uint64_t    frequency;
		uint32_t    probability;
		float       volumeMultiplier;
	};
public:
    EngineSample( AudioSample *sample, SampleProps &props ) :
        m_sample(sample),
		m_props(std::move(props))
    {
    }
    const AudioSample* getSample() const
    {
        return m_sample;
    }
private:
    AudioSample *m_sample;
	SampleProps m_props;
    // TODO list of audio processors to apply
};

