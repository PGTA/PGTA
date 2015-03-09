
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

    float* const __restrict mixBuf = m_mixBuffer.data();
    std::fill(mixBuf, mixBuf + numSamplesToMix, 0.0f);

    float* const __restrict scratchBuf = m_scratchBuffer.data();

    sources.ForEach([=, &sourceMixInfo](SourceMixPair& sourcePair, uint_fast32_t id) -> bool
    {
        akAudioMixer::AudioSource& source = sourcePair.first;
        const uint16_t mixFxBits = sourcePair.second;

        const bool keep = GetSamplesFromSource(source, scratchBuf, numSamplesToMix);

        if (mixFxBits > 0)
        {
            using akAudioMixer::MixEffects;
            using akAudioMixer::MixEffect;
            if (mixFxBits & MixEffectBit(MixEffects::MixEffect_Gain))
            {
                const uint64_t index = MixInfoHash(id, MixEffects::MixEffect_Gain);
                auto it = sourceMixInfo.find(index);
                if (it != sourceMixInfo.end())
                {
                    ProcessGain(scratchBuf, numSamplesToMix, it->second.first.gain);
                    if (!keep)
                    {
                        sourceMixInfo.erase(it);
                    }
                }
            }
            if (mixFxBits & MixEffectBit(MixEffects::MixEffect_Fade))
            {
                const uint64_t index = MixInfoHash(id, MixEffects::MixEffect_Fade);
                auto it = sourceMixInfo.find(index);
                if (it != sourceMixInfo.end())
                {
                    const bool done = !ProcessFade(scratchBuf, numSamplesToMix,
                                                   it->second.first.fade,
                                                   it->second.second.fade);
                    if (done || !keep)
                    {
                        sourceMixInfo.erase(it);
                        sourcePair.second &= ~MixEffectBit(MixEffects::MixEffect_Fade);
                    }
                }
            }
        }

        // VS won't vectorize the loop unless this is used
        static auto AddInto = [](float* const __restrict dest,
                                 float* const __restrict src,
                                 const uint32_t numElems)
        {
            for (uint32_t i = 0; i < numElems; ++i)
            {
                dest[i] += src[i];
            }
        };

        AddInto(mixBuf, scratchBuf, numSamplesToMix);
        return keep;
    });

    TriangleDither triDither;
    for (uint_fast32_t i = 0; i < numSamplesToMix; ++i)
    {
        const int32_t expandedSample = static_cast<int32_t>(triDither(mixBuf[i] * 32768.0f));
        outputBuf[i] = static_cast<int16_t>(std::max(-32768, std::min(expandedSample, 32767)));
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
