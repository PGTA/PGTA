
#include "akAudioMixerImpl.h"
#include <AudioMixer/akAudioSource.h>
#include "akVectorUtils.h"
#include <assert.h>
#include <iostream>

AudioMixerImpl::AudioMixerImpl():
    m_cfg(),
    m_sources(),
    m_mixBuffer(),
    m_mixerTime(0),
    m_userTime(0),
    m_numMixAheadSamples(0)
{
}

bool AudioMixerImpl::Initialize(const akAudioMixer::AudioMixerConfig& cfg)
{
    if (!IsValidConfig(cfg))
    {
        return false;
    }
    const uint32_t numMixAheadSamples =
        static_cast<uint32_t>(cfg.mixAheadSeconds * cfg.sampleFramesPerSecond);

    m_cfg = cfg;
    m_sources.clear();
    m_sources.reserve(64);
    // allocate double the number of mix ahead samples initially
    m_mixBuffer.reserve(numMixAheadSamples << 1);
    m_mixerTime = 0;
    m_userTime = 0;
    m_numMixAheadSamples = numMixAheadSamples;
    return true;
}

AudioMixerImpl::MixHandle AudioMixerImpl::AddSource(const akAudioMixer::AudioSource& source)
{
    m_sources.emplace_back(source);
    return MixHandle{};
}

akAudioMixer::MixControl* AudioMixerImpl::GetMixControl(AudioMixerImpl::MixHandle handle)
{
    return nullptr;
}

akAudioMixer::AudioBuffer AudioMixerImpl::Update(const uint32_t deltaNumSamples)
{
    const uint64_t userTime = m_userTime;
    const uint64_t mixerTime = m_mixerTime;
    const uint32_t numSamplesToMix =
        CalcSamplesToMix(mixerTime, userTime, deltaNumSamples, m_numMixAheadSamples);

    m_mixerTime = mixerTime + numSamplesToMix;
    m_userTime = userTime + deltaNumSamples;

    m_mixBuffer.resize(numSamplesToMix);
    if (m_mixBuffer.capacity() > (m_mixBuffer.size() << 1))
    {
        m_mixBuffer.shrink_to_fit();
    }
    int16_t* outputBuffer = m_mixBuffer.data();

    auto& sources = m_sources;
    std::size_t numSources = sources.size();
    for (std::size_t i = 0; i < numSources; ++i)
    {
        if (!GetSamplesFromSource(sources[i], outputBuffer, numSamplesToMix))
        {
            i = akUtils::FastRemove(sources, i);
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
    std::cout << (count - numReceived) << " silence samples\n";
    return false;
}

uint32_t AudioMixerImpl::CalcSamplesToMix(uint64_t mixerTime, uint64_t userTime,
                                          uint32_t deltaTime, uint32_t mixAheadAmount)
{
    const uint64_t requiredMixerTime = userTime + deltaTime + mixAheadAmount;
    if (requiredMixerTime > mixerTime)
    {
        return static_cast<uint32_t>(requiredMixerTime - mixerTime);
    }
    return 0;
}

bool AudioMixerImpl::IsValidConfig(const akAudioMixer::AudioMixerConfig& cfg)
{
    if (cfg.mixAheadSeconds > 0.5f ||
        cfg.sampleFramesPerSecond != 44100)
    {
        return false;
    }
    return true;
}
