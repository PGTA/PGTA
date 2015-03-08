
#pragma once

#include <AudioMixer/akMixControl.h>
#include <stdint.h>
#include <cmath>

inline float dBToLinear(float dB)
{
    static const float DB_TO_LOG = std::log(10.0f) / 20.0f;
    return std::exp(dB * DB_TO_LOG);
}

inline void ProcessGain(float* samples, uint32_t numSamples,
                        const akAudioMixer::GainEffect& params)
{
    const float linearGain = dBToLinear(params.dBGain);
    for (uint_fast32_t i = 0; i < numSamples; ++i)
    {
        samples[i] *= linearGain;
    }
}
