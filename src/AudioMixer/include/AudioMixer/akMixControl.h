
#pragma once

#include <stdint.h>
#include <AudioMixer/akAudioMixer.h>

namespace akAudioMixer
{
    enum class MixEffects: uint16_t
    {
        MixEffect_Gain = 0,
        MixEffect_Fade,
        MixEffect_NumEffects
    };

    struct GainEffect
    {
        MixEffects type;
        float dBGain;
    };

    struct FadeEffect
    {
        MixEffects type;
        // TODO: add params
    };

    union MixEffect
    {
        MixEffects type;
        GainEffect gain;
        FadeEffect fade;
    };

    class MixControl
    {
    public:
        explicit MixControl(AudioMixer* audioMixer, AudioMixer::MixHandle id);
        void PauseSource(bool resume = false);
        void AddEffect(const MixEffect& effect);
        void RemoveEffect(MixEffects type);

        explicit operator bool() const;

    private:
        AudioMixer* m_audioMixer;
        AudioMixer::MixHandle m_id;
    };

    inline MixControl::MixControl(AudioMixer* audioMixer, AudioMixer::MixHandle id):
        m_audioMixer(audioMixer),
        m_id(id)
    {
    }

    inline void MixControl::PauseSource(bool resume /*= false*/)
    {
        m_audioMixer->PauseSource(m_id, resume);
    }

    inline void MixControl::AddEffect(const akAudioMixer::MixEffect& effect)
    {
        m_audioMixer->AddEffect(m_id, effect);
    }

    inline void MixControl::RemoveEffect(akAudioMixer::MixEffects type)
    {
        m_audioMixer->RemoveEffect(m_id, type);
    }

    inline MixControl::operator bool() const
    {
        return (m_audioMixer && m_id);
    }

}
