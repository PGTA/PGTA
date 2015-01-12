
#pragma once

#include <stdint.h>

class AudioDevice
{
public:
    AudioDevice():
        m_mixTimeAbs(0.0),
        m_mixTimeSamples(0),
        m_samplesPerSecond(44100)
    {
    }

    void Update(const float deltaSec)
    {
        const int64_t numSamplesToMix = SecondsToSamples(deltaSec);
        VerifyTimes(deltaSec, numSamplesToMix);
    }

    void VerifyTimes(const float deltaSec, const int64_t deltaSamples) const
    {
        const int64_t newTimeSamples = SecondsToSamples(m_mixTimeMs + deltaSec);
        const int64_t newTimeSamplesEstimate = m_mixTimeSamples + deltaSamples;

        if (newTimeSamples != newTimeSamplesEstimate)
        {
            printf("AudioDevice::Update: Time count mismatch. expected %i, actual %i\n",
                newTimeSamplesEstimate, newTimeSamples);
        }
    }

    int64_t SecondsToSamples(double sec) const
    {
        return static_cast<int64_t>(sec * m_samplesPerSecond);
    }

    int64_t SecondsToSamples(float sec) const
    {
        return static_cast<int64_t>(sec * m_samplesPerSecond);
    }

private:
    double m_mixTimeMs;
    int64_t m_mixTimeSamples;
    int64_t m_samplesPerSecond;
};
