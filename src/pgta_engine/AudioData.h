
#pragma once

#include <cstdint>
#include <memory>

struct audio_data
{
    audio_data():
        samplesPerSecond(0),
        bytesPerSecond(0),
        blockAlign(0),
        bitsPerSample(0),
        channels(0),
        numSamples(0),
        numSampleBytes(0),
        samples(nullptr)
    {
    }

    audio_data(audio_data &&other):
        samplesPerSecond(other.samplesPerSecond),
        bytesPerSecond(other.bytesPerSecond),
        blockAlign(other.blockAlign),
        bitsPerSample(other.bitsPerSample),
        channels(other.channels),
        numSamples(other.numSamples),
        numSampleBytes(other.numSampleBytes),
        samples(std::move(other.samples))
    {
    }

    audio_data& operator=(audio_data &&other)
    {
        samplesPerSecond = other.samplesPerSecond;
        bytesPerSecond = other.bytesPerSecond;
        blockAlign = other.blockAlign;
        bitsPerSample = other.bitsPerSample;
        channels = other.channels;
        numSamples = other.numSamples;
        numSampleBytes = other.numSampleBytes;
        samples = std::move(other.samples);
        return *this;
    }

    uint32_t samplesPerSecond;
    uint32_t bytesPerSecond;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint16_t channels;
    uint64_t numSamples;
    uint64_t numSampleBytes;
    std::unique_ptr<char[]> samples;
};
