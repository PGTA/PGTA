
#pragma once

#include <memory> 
#include <cstdint>

struct wav_file
{
    char riff[4];
    uint32_t chunk_size;
    char wave[4];
    char fmt[4];
    uint32_t subchunk_size;
    uint16_t format; // 1 = pcm
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];
    uint32_t subchunk2_size;
    std::unique_ptr<int16_t[]> samples;
};

class AudioSample
{
    
public:
    AudioSample( int16_t *samples, uint32_t numSamples, uint32_t sampleRate, uint16_t bitRate, uint16_t channels ):
        m_samples(samples),
        m_numSamples(numSamples),
        m_sampleRate(sampleRate),
        m_bitRate(bitRate),
        m_channels(channels)
    {
    }
    
    int16_t* getSamples() const
    {
        return m_samples;
    }
    uint32_t getSampleRate() const
    {
        return m_sampleRate;
    }
    uint16_t getBitRate() const
    {
        return m_bitRate;
    }
    uint16_t getChannels() const
    {
        return m_channels;
    }
    uint32_t getNumSamples() const
    {
        return m_numSamples;
    }
private:
    std::unique_ptr<int16_t[]> m_samples;
    uint32_t m_sampleRate;
    uint16_t m_bitRate;
    uint16_t m_channels;
    uint32_t m_numSamples;
};

