
#include "akAudioSourceMixer.h"
#include "akDataTable.h"
#include <AudioMixer/akAudioSource.h>
#include <AudioMixer/akMixControl.h>
#include "akMixInfoHash.h"
#include "akDSPEffects.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <random>

class TriangleDither
{
public:
    TriangleDither():
        m_distribution(-0.5f, 0.5f),
        m_generator(),
        m_prevValue(0.0f)
    {
    }

    float operator()(float n)
    {
        const float value = m_distribution(m_generator);
        n = n + value - m_prevValue;
        m_prevValue = value;
        return n;
    }

private:
    std::uniform_real_distribution<float> m_distribution;
    std::mt19937 m_generator;
    float m_prevValue;
};

void AudioSourceMixer::Mix(DataTable<SourceMixPair>& sources,
                           MixInfoMap& sourceMixInfo,
                           int16_t* outputBuf, uint32_t numSamplesToMix)
{
    m_mixBuffer.resize(numSamplesToMix);
    if (m_mixBuffer.capacity() > (numSamplesToMix << 1))
    {
        m_mixBuffer.shrink_to_fit();
    }
    m_scratchBuffer.resize(numSamplesToMix);
    if (m_scratchBuffer.capacity() > (numSamplesToMix << 1))
    {
        m_scratchBuffer.shrink_to_fit();
    }

    float* const mixBuf = m_mixBuffer.data();
    std::fill(mixBuf, mixBuf + numSamplesToMix, 0.0f);

    float* const scratchBuf = m_scratchBuffer.data();

    sources.ForEach([=, &sourceMixInfo](SourceMixPair& data, uint_fast32_t id) -> bool
    {
        akAudioMixer::AudioSource& source = data.first;
        const uint16_t mixFxBits = data.second;

        const bool keep = GetSamplesFromSource(source, scratchBuf, numSamplesToMix);

        if (mixFxBits > 0)
        {
            using akAudioMixer::MixEffects;
            using akAudioMixer::MixEffect;
            if (mixFxBits & (1 << static_cast<uint16_t>(MixEffects::MixEffect_Gain)))
            {
                const uint64_t index = MixInfoHash(id, MixEffects::MixEffect_Gain);
                auto it = sourceMixInfo.find(index);
                if (it != sourceMixInfo.end())
                {
                    ProcessGain(scratchBuf, numSamplesToMix, it->second.gain);
                    if (!keep)
                    {
                        sourceMixInfo.erase(it);
                    }
                }
            }
            if (mixFxBits & (1 << static_cast<uint16_t>(MixEffects::MixEffect_Fade)))
            {
                const uint64_t index = MixInfoHash(id, MixEffects::MixEffect_Fade);
                auto it = sourceMixInfo.find(index);
                if (it != sourceMixInfo.end())
                {
                    // TODO: process effect
                    // ProcessFade(scratchBuf, numSamplesToMix, it->second.fade);
                    if (!keep)
                    {
                        sourceMixInfo.erase(it);
                    }
                }
            }
        }

        for (uint_fast32_t i = 0; i < numSamplesToMix; ++i)
        {
            mixBuf[i] += scratchBuf[i];
        }
        return keep;
    });

    TriangleDither triDither;
    for (uint_fast32_t i = 0; i < numSamplesToMix; ++i)
    {
        const float inSample = mixBuf[i];
        int16_t outSample;
        if (inSample >= 0.0f)
        {
            const int32_t sample = static_cast<int32_t>(triDither(inSample * 32767.0f));
            outSample = (sample <= 32767 ? static_cast<int16_t>(sample) : 32767);
        }
        else
        {
            const int32_t sample = static_cast<int32_t>(triDither(inSample * 32768.0f));
            outSample = (sample >= -32768 ? static_cast<int16_t>(sample) : -32768);
        }
        outputBuf[i] = outSample;
    }
}

bool AudioSourceMixer::GetSamplesFromSource(akAudioMixer::AudioSource& source,
                                            float* output, uint32_t count)
{
    const uint32_t numReceived = source.PopSamples(output, count);
    assert(numReceived <= count);
    if (numReceived == count)
    {
        return true;
    }

    std::fill(output + numReceived, output + count, 0.0f);
    std::cout << (count - numReceived) << " silence samples\n";
    return false;
}
