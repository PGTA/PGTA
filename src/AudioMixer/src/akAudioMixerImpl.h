
#pragma once

#include <AudioMixer/akAudioMixer.h>
#include <AudioMixer/akMixControl.h>
#include "akAudioSourceMixer.h"
#include "akDataTable.h"
#include "akDSPEffectsCommon.h"
#include <vector>
#include <tuple>
#include <map>

namespace akAudioMixer
{
    class AudioSource;
}

class AudioMixerImpl: public akAudioMixer::AudioMixer
{
public:
    AudioMixerImpl();

    bool Initialize(const akAudioMixer::AudioMixerConfig& cfg);

    MixHandle AddSource(const akAudioMixer::AudioSource& source);
    akAudioMixer::MixControl GetMixControl(MixHandle handle);

    uint64_t GetCurTime() const;

    akAudioMixer::AudioBuffer Update(const uint32_t deltaNumSamples);
    akAudioMixer::AudioBuffer GetOutputBuffer();

    // MixControl interface
    void PauseSource(MixHandle handle, bool resume = false);
    void AddEffect(MixHandle handle, const akAudioMixer::MixEffect& effect);
    void RemoveEffect(MixHandle handle, akAudioMixer::MixEffects type);
    //

private:
    static uint32_t CalcSamplesToMix(uint64_t mixerTime, uint64_t userTime,
                                     uint32_t deltaTime, uint32_t mixAheadAmount);
    static bool IsValidConfig(const akAudioMixer::AudioMixerConfig& cfg);

private:
    uint64_t m_mixerTime;
    uint64_t m_userTime;
    uint32_t m_numMixAheadSamples;
    uint32_t m_mixHandleIndexCounter;

    std::vector<int16_t> m_mixBuffer;
    AudioSourceMixer m_sourceMixer;

    using SourceMixPair = std::pair<akAudioMixer::AudioSource, uint16_t>;
    DataTable<SourceMixPair> m_sources;

    using MixInfoMap = std::map<uint64_t, std::pair<akAudioMixer::MixEffect, MixStates>>;
    MixInfoMap m_sourceMixInfo;

    akAudioMixer::AudioMixerConfig m_cfg;
};

inline uint64_t AudioMixerImpl::GetCurTime() const
{
    return m_userTime;
}
