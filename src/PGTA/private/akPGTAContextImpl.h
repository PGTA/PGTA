
#pragma once

#include <private/AudioDevice/akAudioDevice.h>
#include <stdint.h>

struct PGTABuffer;
struct PGTAConfig;

class PGTAContextImpl
{
public:
    PGTAContextImpl();
    ~PGTAContextImpl();

    bool Initialize(const PGTAConfig& config);
    void Shutdown();

    PGTABuffer* Update(const float delta, int32_t* numOutputBuffers);

    PGTABuffer* GetOutputBuffers(int32_t* numOutputBuffers);

private:
    AudioDevice m_audioDevice;
};
