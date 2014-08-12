
#pragma once

#include <cstdint>
#include <string>
#include "PGTAOutputBuffer.h"

namespace PGTA
{
    struct PGTAConfig
    {
        AudioDesc audioDesc;
        uint16_t bufferSize;
    };

    class IPGTA
    {
    public:
        virtual ~IPGTA()
        {
        }

        // Initialize PGTA
        virtual void Initialize(const PGTAConfig &config) = 0;

        // Enables PGTA to generate audio data on update calls
        virtual bool StartPlayback(const std::string &trackName) = 0;

        // Disables PGTA audio generation
        virtual void StopPlayback() = 0;

        // Triggers transition event with the amount of transitionAmount [0,255]
        virtual void TransitionEvent(const std::string &event, uint8_t transitionAmount) = 0;

        // Call before grabbing audio data
        virtual void Update() = 0;

        // Returns an array of buffers with the latest audio data.
        // Buffers become invalid after the next call to Update().
        virtual const OutputBuffer* GetOutputBuffers(int& numBuffers) const = 0;
    };

    IPGTA* CreatePGTA();

    void FreePGTA(IPGTA* pgta);
}
