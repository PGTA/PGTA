
#pragma once

#include <AudioMixer/akAudioMixer.h>
#include <AudioMixer/akAudioSource.h>
#include <vector>

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
    bool GetSamplesFromSource(akAudioMixer::AudioSource& source, int16_t* output, uint32_t count);
    uint32_t CalcSamplesToMix(uint64_t mixerTime, uint64_t userTime,
                              uint32_t deltaTime, uint32_t mixAheadAmount);

private:
    akAudioMixer::AudioMixerConfig m_cfg;

    std::vector<akAudioMixer::AudioSource> m_sources;
    std::vector<int16_t> m_mixBuffer;

    uint64_t m_mixerTime;
    uint64_t m_userTime;
    uint32_t m_numMixAheadSamples;
};
