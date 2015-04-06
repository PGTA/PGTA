
#pragma once

#include <AudioMixer/akMixControl.h>
#include "akDSPEffectsCommon.h"
#include <algorithm>
#include <stdint.h>
#include <assert.h>
#include <cmath>

inline void ProcessGain(float* samples, uint32_t numSamples,
                        const akAudioMixer::GainEffect& params)
{
    const float linearGain = dBToLinear(params.dBGain);
    for (uint_fast32_t i = 0; i < numSamples; ++i)
    {
        samples[i] *= linearGain;
    }
}

namespace DSPLocal
{
    inline float min(float left, float right)
    {
        return (left < right) ? left : right;
    }

    inline float max(float left, float right)
    {
        return (left < right) ? right : left;
    }

    inline float clamp(float num, float low, float high)
    {
        return max(min(num, high), low);
    }

    inline float lerp(float from, float to, float factor)
    {
        // lerp: from*(1.0f-factor) + to*factor
        // from*(1.0f-factor) == from - from*factor == -from*factor + from
        // fma: x*y+z
        //return std::fma(to, factor, std::fma(-from, factor, from));
        return from*(1.0f-factor) + to*factor;
    };
}

inline bool ProcessFade(float* samples, uint32_t numSamples,
                        const akAudioMixer::FadeEffect& params,
                        FadeState& fadeState)
{
    assert(fadeState.fadeSamplesSoFar < params.fadeOverNumSamples);
    //assert(numSamples > 0);

    const uint32_t fadeSamplesOffset = fadeState.fadeSamplesSoFar;
    const uint32_t fadeSamplesTotal = params.fadeOverNumSamples;
    const float linearInitial = dBToLinear(params.dBInitial);
    const float linearFinal = dBToLinear(params.dBFinal);

    const float fadeFactorInc = (linearFinal - linearInitial) / fadeSamplesTotal;
    float fadeFactor = std::fma(fadeFactorInc,
        static_cast<float>(fadeSamplesOffset), linearInitial);
    for (uint_fast32_t i = 0; i < numSamples; ++i)
    {
        samples[i] *= fadeFactor;
        fadeFactor += fadeFactorInc;
        // TODO: add clamp if addition error is an issue
    }
    fadeState.fadeSamplesSoFar += numSamples;
    return (fadeSamplesOffset + numSamples) < fadeSamplesTotal;
}
