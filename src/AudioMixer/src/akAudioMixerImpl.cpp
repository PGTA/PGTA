
#include "akAudioMixerImpl.h"

AudioMixerImpl::MixHandle AudioMixerImpl::AddSample(AudioSample sample)
{
    return MixHandle{};
}

akAudioMixer::MixControl* AudioMixerImpl::GetMixControl(AudioMixerImpl::MixHandle handle)
{
    return nullptr;
}

akAudioMixer::AudioBuffer AudioMixerImpl::Update(const uint32_t deltaNumSamples)
{


    akAudioMixer::AudioBuffer output;
    output.samples = m_outputBuffer.get();
    output.numSamples = 0;
    return output;
}

akAudioMixer::AudioBuffer AudioMixerImpl::GetOutputBuffer()
{
    return akAudioMixer::AudioBuffer{};
}
