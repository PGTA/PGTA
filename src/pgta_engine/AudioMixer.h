
#pragma once

#include "AudioStreamBuffer.h"
#include "SDL_audio.h"
#include <chrono>
#include <cassert>
#include <vector>
#include "RealToInt.h"
#include <iostream>

class AudioSteamBuffer;

template<int BITS_PER_SAMPLE>
class AudioMixer
{
    using TimeDuration = std::chrono::high_resolution_clock::duration;
public:
    int Mix(char* outputBuffer, int numSamplesToMix, int numBytesToMix)
    {
        int bytesPerSample = BITS_PER_SAMPLE / 8;
        memset(outputBuffer, 0, numBytesToMix);

        int minSampleCount = 0;
        int samplesMixed = 0;
        while (((minSampleCount = GetMinInputSampleCount()) > 0)
               && (samplesMixed < numSamplesToMix))
        {
            int samplesToMix = std::min(numSamplesToMix, minSampleCount);
            IncrementalMix(numSamplesToMix - samplesMixed, bytesPerSample, outputBuffer);
            outputBuffer += (samplesToMix*bytesPerSample);
            samplesMixed += samplesToMix;
        }
        return numSamplesToMix;
    }

    void ConnectInputStream( AudioStreamBuffer* inputStream )
    {
        assert(BITS_PER_SAMPLE == inputStream->GetBitsPerSample());
        m_inputStreams.push_back(inputStream);
    }

private:
    void IncrementalMix(int numSamplesToMix, int bytesPerSample, char* dstBuffer)
    {
        int totalBytes = numSamplesToMix * bytesPerSample;
        m_inputBuffer.resize(totalBytes, 0);
        for (auto *inputStream : m_inputStreams)
        {
            std::fill(m_inputBuffer.begin(), m_inputBuffer.end(), 0);
            int numGivenSamples = inputStream->PullSamples(m_inputBuffer.data(), numSamplesToMix);
            if (numGivenSamples == 0)
            {
                continue;
            }
            //assert(numGivenSamples == numSamplesToMix);
            // HACKHACKHACK - AUDIO_S16 need to be determined for all BPS values
            SDL_MixAudioFormat((uint8_t*)dstBuffer, (uint8_t*)m_inputBuffer.data(),
                               AUDIO_S16, totalBytes, SDL_MIX_MAXVOLUME);
        }
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
