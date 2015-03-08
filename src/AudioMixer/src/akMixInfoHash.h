
#pragma once

#include <stdint.h>
#include <AudioMixer/akMixControl.h>

inline uint64_t MixInfoHash(uint32_t handleId, akAudioMixer::MixEffects type)
{
    using akAudioMixer::MixEffects;
    const uint16_t typeValue = static_cast<uint16_t>(type);
    const uint16_t numEffects = static_cast<uint16_t>(MixEffects::MixEffect_NumEffects);
    return (handleId * numEffects) + typeValue;
}
