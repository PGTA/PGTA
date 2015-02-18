
#pragma once

class MixControlImpl;

namespace akAudioMixer
{

    class MixControl
    {
    public:
        using Impl = MixControlImpl;

        void PauseAudio(bool resume = false);

        void AddEffect();
        void RemoveEffect();
    };

}
