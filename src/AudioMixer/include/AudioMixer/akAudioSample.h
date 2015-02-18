
#pragma once

#include <stdint.h>
#include <memory>

struct AudioSample
{
    std::shared_ptr<int16_t[]> samples;
    uint64_t numSamples;
    uint32_t samplesPerSecond;
    uint8_t numChannels;
};
