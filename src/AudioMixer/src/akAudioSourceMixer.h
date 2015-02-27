
#pragma once

#include <AudioMixer/akAudioSource.h>
#include <stdint.h>
#include <vector>

namespace akAudioMixer
{
    class AudioSource;
}

class AudioSourceMixer
{
public:
    void Mix(std::vector<akAudioMixer::AudioSource>& sources,
             int16_t* outputBuf, uint32_t numSamplesToMix);

private:
    bool GetSamplesFromSource(akAudioMixer::AudioSource& source,
                              int32_t* output, uint32_t count);

private:
    std::vector<int32_t> m_mixBuffer;
};
