
#include "akAudioMixerImpl.h"
#include "akVectorUtils.h"
#include <assert.h>
#include <iostream>

AudioMixerImpl::AudioMixerImpl():
    m_cfg(),
    m_sources(),
    m_mixBuffer(),
    m_numMixAheadSamples(0),
    m_mixerTime(0),
    m_userTime(0)
{
}

bool AudioMixerImpl::Initialize(const akAudioMixer::AudioMixerConfig& cfg)
{
    uint64_t numMixAheadSamples = static_cast<uint64_t>(cfg.mixAheadSeconds * 44100);

    m_cfg = cfg;
    m_sources.clear();
    m_sources.reserve(64);
    // allocate double the number of mix ahead samples initially
    m_mixBuffer.reserve(numMixAheadSamples << 1);
    m_numMixAheadSamples = numMixAheadSamples;
    m_mixerTime = 0;
    m_userTime = 0;
    return true;
}

AudioMixerImpl::MixHandle AudioMixerImpl::AddSource(akAudioMixer::AudioSource source)
{
    m_sources.emplace_back(std::move(source));
    return MixHandle{};
}

akAudioMixer::MixControl* AudioMixerImpl::GetMixControl(AudioMixerImpl::MixHandle handle)
{
    return nullptr;
}

akAudioMixer::AudioBuffer AudioMixerImpl::Update(const uint32_t deltaNumSamples)
{
    uint64_t userTime = m_userTime;
    uint64_t mixerTime = m_mixerTime;
    uint64_t numSamplesToMix = CalcSamplesToMix(mixerTime, userTime, deltaNumSamples, m_numMixAheadSamples);

    m_mixerTime = mixerTime + numSamplesToMix;
    m_userTime = userTime + deltaNumSamples;

    if (m_sources.empty())
    {
        return akAudioMixer::AudioBuffer{};
    }

    std::cout << mixerTime << " " << userTime << " " << deltaNumSamples << " " << m_numMixAheadSamples << " "
              << "Mixing " << numSamplesToMix << " samples" << std::endl;

    m_mixBuffer.resize(numSamplesToMix);
    int16_t* outputBuffer = m_mixBuffer.data();

    auto& sources = m_sources;
    std::size_t numSources = sources.size();
    for (std::size_t i = 0; i < numSources; ++i)
    {
        if (!GetSamplesFromSource(sources[i], outputBuffer, numSamplesToMix))
        {
            akUtils::FastRemove(sources, i);
            --i;
            --numSources;
            continue;
        }
    }

    akAudioMixer::AudioBuffer output;
    output.samples = outputBuffer;
    output.numSamples = numSamplesToMix;
    return output;
}

akAudioMixer::AudioBuffer AudioMixerImpl::GetOutputBuffer()
{
    return akAudioMixer::AudioBuffer{};
}

bool AudioMixerImpl::GetSamplesFromSource(akAudioMixer::AudioSource& source, int16_t* output, uint32_t count)
{
    const uint32_t numReceived = source.PopSamples(output, count);
    assert(numReceived <= count);
    if (numReceived == count)
    {
        return true;
    }

    std::fill(output + numReceived, output + count, 0);
    std::cout << (count - numReceived) << " silence samples" << std::endl;
    return false;
}

uint64_t AudioMixerImpl::CalcSamplesToMix(uint64_t mixerTime, uint64_t userTime,
                                          uint32_t deltaTime, uint64_t mixAheadAmount)
{
    const uint64_t requiredMixerTime = userTime + deltaTime + mixAheadAmount;
    if (requiredMixerTime > mixerTime)
    {
        return requiredMixerTime - mixerTime;
    }
    return 0;
}
