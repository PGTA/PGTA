
#pragma once

#include "akDataTable.h"
#include <AudioMixer/akAudioSource.h>
#include <AudioMixer/akMixControl.h>
#include <stdint.h>
#include <vector>
#include <map>

class AudioSourceMixer
{
    using SourceMixPair = std::pair<akAudioMixer::AudioSource, uint16_t>;
    using MixInfoMap = std::map<uint64_t, akAudioMixer::MixEffect>;
public:
    void Mix(DataTable<SourceMixPair>& sources,
             const MixInfoMap& sourceMixInfo,
             int16_t* outputBuf, uint32_t numSamplesToMix);

private:
    bool GetSamplesFromSource(akAudioMixer::AudioSource& source,
                              float* output, uint32_t count);

private:
    std::vector<float> m_mixBuffer;
    std::vector<float> m_scratchBuffer;
};
