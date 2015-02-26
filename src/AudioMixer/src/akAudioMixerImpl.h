
#pragma once

#include <AudioMixer/akAudioMixer.h>
#include "akAudioSourceMixer.h"
#include <vector>

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
    akAudioMixer::MixControl* GetMixControl(MixHandle handle);

    akAudioMixer::AudioBuffer Update(const uint32_t deltaNumSamples);
    akAudioMixer::AudioBuffer GetOutputBuffer();

private:
    uint32_t CalcSamplesToMix(uint64_t mixerTime, uint64_t userTime,
                              uint32_t deltaTime, uint32_t mixAheadAmount);
    static bool IsValidConfig(const akAudioMixer::AudioMixerConfig& cfg);

private:
    akAudioMixer::AudioMixerConfig m_cfg;

    std::vector<akAudioMixer::AudioSource> m_sources;
    std::vector<int16_t> m_mixBuffer;

    uint64_t m_mixerTime;
    uint64_t m_userTime;
    uint32_t m_numMixAheadSamples;

    AudioSourceMixer m_sourceMixer;
};
