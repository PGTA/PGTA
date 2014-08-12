
#pragma once

#include "AudioStreamBuffer.h"
#include "SDL_audio.h"
#include <chrono>
#include <cassert>
#include <vector>
#include "RealToInt.h"

class AudioSteamBuffer;

template<int BITS_PER_SAMPLE>
class AudioMixer
{
    using TimeDuration = std::chrono::high_resolution_clock::duration;
public:
    int Mix(TimeDuration dt,
            std::vector<char>& outputBuffer)
    {
        int bytesPerSample = BITS_PER_SAMPLE / 8;
        int totalSamplesToMix = CalcNumSamplesToMix(dt);

        outputBuffer.resize(totalSamplesToMix * bytesPerSample);
        std::fill(outputBuffer.begin(), outputBuffer.end(), 0);

        int minSampleCount = 0;
        int samplesMixed = 0;
        uint8_t* dstBuffer = reinterpret_cast<uint8_t*>(outputBuffer.data());
        while ((minSampleCount = GetMinInputSampleCount()) > 0
               && (samplesMixed < totalSamplesToMix))
        {
            int samplesToMix = std::min(totalSamplesToMix, minSampleCount);
            IncrementalMix(samplesToMix, bytesPerSample, dstBuffer);
            dstBuffer += (samplesToMix*bytesPerSample);
            samplesMixed += samplesToMix;
        }
        return totalSamplesToMix;
    }

    void ConnectInputStream( AudioStreamBuffer* inputStream )
    {
        assert(BITS_PER_SAMPLE == inputStream->GetBitsPerSample());
        m_inputStreams.push_back(inputStream);
    }

private:
    void IncrementalMix(int numSamples, int bytesPerSample,
                        uint8_t* dstBuffer)
    {
        m_inputBuffer.resize(numSamples * bytesPerSample, 0);
        for (auto *inputStream : m_inputStreams)
        {
            int numGivenSamples = inputStream->PullSamples(m_inputBuffer.data(), numSamples);
            uint8_t* srcBuffer = reinterpret_cast<uint8_t*>(m_inputBuffer.data());

            // HACKHACKHACK - AUDIO_S16 need to be determined for all BPS values
            SDL_MixAudioFormat(dstBuffer, srcBuffer, AUDIO_S16,
                               numGivenSamples*bytesPerSample, SDL_MIX_MAXVOLUME);
        }
    }

    int CalcNumSamplesToMix(TimeDuration dt)
    {
        using namespace std::chrono;
        double seconds = duration_cast<duration<double>>(dt).count();
        // TODO: get rid of hardcoded sample rate
        return Real2Int(seconds * 44100.0);
    }

    int GetMinInputSampleCount()
    {
        int minSampleCount = std::numeric_limits<int>::max();
        for (auto *inputStream : m_inputStreams)
        {
            auto numSamples = inputStream->GetNumSamples();
            if (numSamples > 0)
            {
                minSampleCount = std::min(minSampleCount, numSamples);
            }
        }

        if (minSampleCount == std::numeric_limits<int>::max())
        {
            return 0;
        }
        return minSampleCount;
    }

private:
    std::vector<char> m_inputBuffer;
    std::vector<AudioStreamBuffer*> m_inputStreams;
};
