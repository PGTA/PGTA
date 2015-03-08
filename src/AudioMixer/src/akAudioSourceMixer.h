
#pragma once

#include <stdint.h>
#include <vector>
#include "akDataTable.h"
#include <AudioMixer/akAudioSource.h>
#include <AudioMixer/akMixControl.h>

class AudioSourceMixer
{
    using SourceMixPair = std::pair<akAudioMixer::AudioSource, uint16_t>;
public:
    void Mix(DataTable<SourceMixPair>& sources,
             int16_t* outputBuf, uint32_t numSamplesToMix);

private:
    bool GetSamplesFromSource(akAudioMixer::AudioSource& source,
                              float* output, uint32_t count);

private:
    std::vector<float> m_mixBuffer;
    std::vector<float> m_scratchBuffer;
};
