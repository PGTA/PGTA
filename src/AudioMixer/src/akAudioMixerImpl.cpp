
#include "akAudioMixerImpl.h"
#include <AudioMixer/akAudioSource.h>
#include <AudioMixer/akMixControl.h>
#include "akMixInfoHash.h"

AudioMixerImpl::AudioMixerImpl():
    m_mixerTime(0),
    m_userTime(0),
    m_numMixAheadSamples(0),
    m_mixHandleIndexCounter(0),
    m_mixBuffer(),
    m_sourceMixer(),
    m_sources(),
    m_sourceMixInfo(),
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
    m_sourceMixInfo.clear();
    m_cfg = cfg;

    return true;
}

AudioMixerImpl::MixHandle AudioMixerImpl::AddSource(const akAudioMixer::AudioSource& source)
{
    const uint32_t id = ++m_mixHandleIndexCounter;
    SourceMixPair temp = std::make_pair(source, 0);
    m_sources.AddData(id, &temp);
    return MixHandle(id);
}

akAudioMixer::MixControl AudioMixerImpl::GetMixControl(AudioMixerImpl::MixHandle handle)
{
    if (!handle || !m_sources.HasData(handle.m_id))
    {
        return akAudioMixer::MixControl(nullptr, AudioMixerImpl::MixHandle());
    }
    return akAudioMixer::MixControl(this, handle);
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

    m_sourceMixer.Mix(m_sources, m_sourceMixInfo, outputBuffer, numSamplesToMix);

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

void AudioMixerImpl::PauseSource(MixHandle handle, bool /*resume*/ /*= false*/)
{
    assert(false);
    SourceMixPair* source = nullptr;
    if (!handle || (source = m_sources.GetData(handle.m_id)) == nullptr)
    {
        return;
    }
}

void AudioMixerImpl::AddEffect(MixHandle handle, const akAudioMixer::MixEffect& effect)
{
    using akAudioMixer::MixEffects;
    SourceMixPair* source = nullptr;
    if (!handle || (effect.type >= MixEffects::MixEffect_NumEffects) ||
        (source = m_sources.GetData(handle.m_id)) == nullptr)
    {
        return;
    }

    source->second |= (1 << static_cast<uint16_t>(effect.type));
    const uint64_t mixInfoIndex = MixInfoHash(handle.m_id, effect.type);
    m_sourceMixInfo[mixInfoIndex] = effect;
}

void AudioMixerImpl::RemoveEffect(MixHandle handle, akAudioMixer::MixEffects type)
{
    using akAudioMixer::MixEffects;
    SourceMixPair* source = nullptr;
    if (!handle || (type >= MixEffects::MixEffect_NumEffects) ||
        (source = m_sources.GetData(handle.m_id)) == nullptr)
    {
        return;
    }

    source->second &= ~(1 << static_cast<uint16_t>(type));
    const uint64_t mixInfoIndex = MixInfoHash(handle.m_id, type);
    m_sourceMixInfo.erase(mixInfoIndex);
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
