
#include "akAudioMixerImpl.h"
#include <AudioMixer/akAudioSource.h>
#include <AudioMixer/akMixControl.h>

AudioMixerImpl::AudioMixerImpl():
    m_mixerTime(0),
    m_userTime(0),
    m_numMixAheadSamples(0),
    m_mixHandleIndexCounter(0),
    m_mixBuffer(),
    m_sourceMixer(),
    m_sources(),
    m_cfg()
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

    m_mixerTime = 0;
    m_userTime = 0;
    m_numMixAheadSamples = numMixAheadSamples;
    m_mixHandleIndexCounter = 0;

    // allocate double the number of mix ahead samples initially
    m_mixBuffer.reserve(numMixAheadSamples << 1);
    m_sources.Clear();
    m_cfg = cfg;

    return true;
}

AudioMixerImpl::MixHandle AudioMixerImpl::AddSource(const akAudioMixer::AudioSource& source)
{
    const uint32_t id = ++m_mixHandleIndexCounter;
    SourceMixPair temp = std::make_pair(source, akAudioMixer::MixControl());
    m_sources.AddData(id, &temp);
    return MixHandle{id};
}

akAudioMixer::MixControl* AudioMixerImpl::GetMixControl(AudioMixerImpl::MixHandle handle)
{
    if (!handle)
    {
        return nullptr;
    }

    SourceMixPair* source = m_sources.GetData(handle.id);
    if (!source)
    {
        return nullptr;
    }

    return &source->second;
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
    if (m_mixBuffer.capacity() > (numSamplesToMix << 1))
    {
        m_mixBuffer.shrink_to_fit();
    }
    int16_t* outputBuffer = m_mixBuffer.data();

    m_sourceMixer.Mix(m_sources, outputBuffer, numSamplesToMix);

    akAudioMixer::AudioBuffer output;
    output.samples = outputBuffer;
    output.numSamples = numSamplesToMix;
    return output;
}

akAudioMixer::AudioBuffer AudioMixerImpl::GetOutputBuffer()
{
    akAudioMixer::AudioBuffer output;
    output.samples = m_mixBuffer.data();
    output.numSamples = m_mixBuffer.size();
    return output;
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
