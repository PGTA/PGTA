
#pragma once

#include <stdint.h>

class AudioDevice
{
public:
    AudioDevice();

    void Update(const float deltaSec);

private:
    void VerifyTimes(const float deltaSec, const int64_t deltaSamples) const;

    int64_t SecondsToSamples(double sec) const;
    int64_t SecondsToSamples(float sec) const;

private:
    double m_mixTimeMs;
    int64_t m_mixTimeSamples;
    int64_t m_samplesPerSecond;
};

inline AudioDevice::AudioDevice():
    m_mixTimeMs(0.0),
    m_mixTimeSamples(0),
    m_samplesPerSecond(44100)
{
}

inline int64_t AudioDevice::SecondsToSamples(double sec) const
{
    return static_cast<int64_t>(sec * m_samplesPerSecond);
}

inline int64_t AudioDevice::SecondsToSamples(float sec) const
{
    return static_cast<int64_t>(sec * m_samplesPerSecond);
}
