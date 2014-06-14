
#pragma once

#include <cstdint>
#include <memory>

struct audio_data
{
    uint32_t samplesPerSecond;
    uint32_t bytesPerSecond;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint16_t channels;
    uint64_t numSamples;
    uint64_t numSampleBytes;
    std::unique_ptr<char[]> samples;
};
