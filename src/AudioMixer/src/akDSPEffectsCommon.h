
#pragma once

#include <AudioMixer/akMixControl.h>
#include <stdint.h>
#include <cmath>

struct FadeState
{
    uint32_t fadeSamplesSoFar;
};

union MixStates
{
    FadeState fade;
};

inline float dBToLinear(float dB)
{
    static const float DB_TO_LOG = std::log(10.0f) / 20.0f;
    return std::exp(dB * DB_TO_LOG);
}

inline uint16_t MixEffectBit(akAudioMixer::MixEffects type)
{
    return (1 << static_cast<uint16_t>(type));
}
