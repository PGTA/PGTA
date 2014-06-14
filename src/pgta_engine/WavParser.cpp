
#include "WavParser.h"
#include "AudioData.h"
#include <cstdio>
#include <cstdint>

struct riff_header
{
    char riff[4];
    uint32_t chunk_size;
    char wave[4];
};

struct subchunk_header
{
    union
    {
        uint32_t name_id;
        char name[4];
    };
    uint32_t size;
};

struct subchunk_fmt
{
    uint16_t format; // 1 = pcm
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

bool ParseWaveFile( const char *filePath, audio_data &dest )
{
    FILE *wavFile = fopen(filePath, "rb");
    if (!wavFile)
    {
        return false;
    }

    riff_header riff;
    fread(&riff, 1, sizeof(riff_header), wavFile);

    while (ftell(wavFile) < riff.chunk_size+4)
    {
        subchunk_header header;
        fread(&header, 1, sizeof(header), wavFile);
        // std::cout << std::string(header.name, sizeof(header.name)) << std::endl;
        // printf("0x%x\n", header.name_id);
        switch (header.name_id)
        {
        // "fmt "
        case 0x20746d66:
            {
                subchunk_fmt fmt;
                fread(&fmt, 1, sizeof(subchunk_fmt), wavFile);
                // TODO: do the format==1 thing
                if (fmt.format != 1)
                {
                    uint16_t extraData = 0;
                    fread(&extraData, 1, sizeof(uint16_t), wavFile);
                    fseek(wavFile, extraData, SEEK_CUR);
                }
                
                dest.samplesPerSecond = fmt.sample_rate;
                dest.bytesPerSecond = fmt.byte_rate;
                dest.blockAlign = fmt.block_align;
                dest.bitsPerSample = fmt.bits_per_sample;
                dest.channels = fmt.channels;
            }
            break;
        // "data"
        case 0x61746164:
            {
                dest.numSampleBytes = header.size;
                dest.samples.reset(new char[header.size]());
                fread(dest.samples.get(), 1, header.size, wavFile);
            }
            break;
        default:
            // std::cout << "skipping subchunk header" << std::endl;
            fseek(wavFile, header.size, SEEK_CUR);
            break;
        }
    }

    // set dest.numSamples here
    dest.numSamples = dest.numSampleBytes / (dest.bitsPerSample / 8);

    fclose(wavFile);
    return true;
}
