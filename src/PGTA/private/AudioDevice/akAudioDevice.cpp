
#include <private/AudioDevice/akAudioDevice.h>
#include <stdio.h>
#include <inttypes.h>

void AudioDevice::Update(const float deltaSec)
{
    const int64_t numSamplesToMix = SecondsToSamples(deltaSec);
    VerifyTimes(deltaSec, numSamplesToMix);
}

void AudioDevice::VerifyTimes(const float deltaSec, const int64_t deltaSamples) const
{
    const int64_t newTimeSamples = SecondsToSamples(m_mixTimeMs + deltaSec);
    const int64_t newTimeSamplesEstimate = m_mixTimeSamples + deltaSamples;

    if (newTimeSamples != newTimeSamplesEstimate)
    {
        printf("AudioDevice::Update: Time count mismatch. expected %" PRIu64 ", actual %" PRIu64 "\n",
               newTimeSamplesEstimate, newTimeSamples);
    }
}
