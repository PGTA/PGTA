
#include "akAudioSourceMixer.h"
#include "akVectorUtils.h"
#include <limits>
#include <algorithm>
#include <assert.h>
#include <iostream>

template<typename T>
T clamp(const T& n, const T& lower, const T& upper) {
    return std::max(lower, std::min(n, upper));
}

void AudioSourceMixer::Mix(std::vector<akAudioMixer::AudioSource>& sources,
                           int16_t* outputBuf, uint32_t numSamplesToMix)
{
    m_mixBuffer.resize(numSamplesToMix);
    if (m_mixBuffer.capacity() > (numSamplesToMix << 1))
    {
        m_mixBuffer.shrink_to_fit();
    }

    int32_t* mixBuf = m_mixBuffer.data();
    std::fill_n(mixBuf, numSamplesToMix, 0);

    std::size_t numSources = sources.size();
    for (std::size_t i = 0; i < numSources; ++i)
    {
        if (!GetSamplesFromSource(sources[i], mixBuf, numSamplesToMix))
        {
            i = akUtils::FastRemove(sources, i);
            --numSources;
            continue;
        }
    }

    std::transform(mixBuf, mixBuf + numSamplesToMix, outputBuf, [](int32_t n)
    {
        using std::numeric_limits;
        return clamp(n,
            static_cast<int32_t>(numeric_limits<int16_t>::min()),
            static_cast<int32_t>(numeric_limits<int16_t>::max()));
    });
}

bool AudioSourceMixer::GetSamplesFromSource(akAudioMixer::AudioSource& source,
                                            int32_t* output, uint32_t count)
{
    const uint32_t numReceived = source.PopAddSamples(output, count);
    assert(numReceived <= count);
    if (numReceived == count)
    {
        return true;
    }

    //std::fill(output + numReceived, output + count, 0);
    std::cout << (count - numReceived) << " silence samples\n";
    return false;
}
