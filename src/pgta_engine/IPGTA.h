
#pragma once

#include <cstdint>

struct PGTAConfig
{
    uint32_t samplesPerSecond;
    uint16_t bitsPerSample;
    uint16_t channels;
    uint64_t numSamples;
};

class IPGTA
{
    
public:
    // Initialize PGTA
    virtual void Initialize(const PGTAConfig &config) = 0;
    
    // Enables PGTA to generate audio data on update calls
    virtual void StartPlayback(const std::string &trackName) = 0;
    
    // Disables PGTA audio generation
    virtual void StopPlayback() = 0;
    
    // Triggers transition event with the amount of transitionAmount [0,255]
    virtual void TrasitionEvent(const std::string &event, uint8_t transitionAmount) = 0;

    // Call before grabbing audio data
    virtual void Update() = 0;
    
    // Puts at most length amount of bytes in buffer
    // Returns: How many bytes actually sent
    virtual int GetAudioData(char *buffer, int length) = 0;

};
