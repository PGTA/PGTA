
#pragma once

#include <cstdint>

namespace PGTA
{
    struct AudioDesc
    {
        uint32_t samplesPerSecond;
        uint16_t bitsPerSample;
        uint16_t channels;
    };

    struct OutputBuffer
    {
        const AudioDesc* audioDesc;
        const char* audioData;
        int32_t numSamples;
    };
}
