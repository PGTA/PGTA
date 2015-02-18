
#pragma once

#include "AudioMixer/akAudioMixer.h"

class AudioMixerImpl: public akAudioMixer::AudioMixer
{
public:
    MixHandle AddSample(AudioSample sample);
    akAudioMixer::MixControl* GetMixControl(MixHandle handle);

    akAudioMixer::AudioBuffer Update(const uint32_t deltaNumSamples);
    akAudioMixer::AudioBuffer GetOutputBuffer();

private:
    std::unique_ptr<int16_t[]> m_outputBuffer;
    uint64_t m_outputBufferSize;
};
