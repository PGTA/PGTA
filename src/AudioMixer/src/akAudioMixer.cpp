
#include "AudioMixer/akAudioMixer.h"
#include "akAudioMixerCommon.h"
#include "akAudioMixerImpl.h"

namespace akAudioMixer
{
    AudioMixer::MixHandle AudioMixer::AddSource(const AudioSource& source)
    {
        return ToImpl(this)->AddSource(source);
    }

    MixControl AudioMixer::GetMixControl(AudioMixer::MixHandle handle)
    {
        return ToImpl(this)->GetMixControl(handle);
    }

    uint64_t AudioMixer::GetCurTime() const
    {
        return ToImpl(this)->GetCurTime();
    }

    AudioBuffer AudioMixer::Update(const uint32_t deltaNumSamples)
    {
        return ToImpl(this)->Update(deltaNumSamples);
    }

    AudioBuffer AudioMixer::GetOutputBuffer()
    {
        return ToImpl(this)->GetOutputBuffer();
    }

    void AudioMixer::PauseSource(MixHandle handle, bool resume /*= false*/)
    {
        ToImpl(this)->PauseSource(handle, resume);
    }

    void AudioMixer::AddEffect(MixHandle handle, const akAudioMixer::MixEffect& effect)
    {
        ToImpl(this)->AddEffect(handle, effect);
    }

    void AudioMixer::RemoveEffect(MixHandle handle, akAudioMixer::MixEffects type)
    {
        ToImpl(this)->RemoveEffect(handle, type);
    }

    AudioMixer* CreateAudioMixer(const AudioMixerConfig& cfg)
    {
        AudioMixerImpl* mixer = new AudioMixerImpl();
        if (mixer && mixer->Initialize(cfg))
        {
            return mixer;
        }
        delete mixer;
        return nullptr;
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
