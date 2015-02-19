
#include "AudioMixer/akAudioMixer.h"
#include "akAudioMixerCommon.h"
#include "akAudioMixerImpl.h"

namespace akAudioMixer
{
    AudioMixer::MixHandle AudioMixer::AddSample(AudioSample sample)
    {
        return ToImpl(this)->AddSample(sample);
    }

    MixControl* AudioMixer::GetMixControl(AudioMixer::MixHandle handle)
    {
        return ToImpl(this)->GetMixControl(handle);
    }

    AudioBuffer AudioMixer::Update(const uint32_t deltaNumSamples)
    {
        return ToImpl(this)->Update(deltaNumSamples);
    }

    AudioBuffer AudioMixer::GetOutputBuffer()
    {
        return ToImpl(this)->GetOutputBuffer();
    }

    AudioMixer* CreateAudioMixer(const AudioMixerConfig& cfg)
    {
        return new AudioMixerImpl();
    }

    void FreeAudioMixer(AudioMixer*& mixer)
    {
        if (mixer)
        {
            delete ToImpl(mixer);
            mixer = nullptr;
        }
    }
}
