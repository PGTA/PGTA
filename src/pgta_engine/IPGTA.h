
#pragma once

#include <cstdint>
#include <string>
#include "PGTAOutputBuffer.h"

namespace PGTA
{
    struct PGTAConfig
    {
        AudioDesc audioDesc;

        uint16_t numBuffers;
        uint16_t bufferSizeInSamples;
    };

    struct PGTATrackHandle
    {
        PGTATrackHandle(int16_t instNum) : instanceNumber(instNum) {}
        const int16_t instanceNumber;
    };

    class IPGTA
    {
    public:
        virtual ~IPGTA()
        {
        }

        // Initialize PGTA
        virtual void Initialize(const PGTAConfig &config) = 0;

        // Loads a track into PGTA so that it is ready to play (returns handle with instance number -1 if an invalid track is given) 
        virtual PGTATrackHandle const LoadTrack(const std::string &trackName) = 0;
        
        // Releases the track with the specified instance number from PGTA
        virtual void FreeTrack(const PGTATrackHandle instance) = 0;

        // Start generating audio data on update calls for the specifed track
        virtual bool PlayTrack(const PGTATrackHandle instance) = 0;

        // Enables PGTA to generate audio data on update calls
        virtual void StartPlayback() = 0;

        // Disables PGTA audio generation
        virtual void StopPlayback() = 0;

        // Triggers transition event with the amount of transitionAmount [0,255]
        virtual void TransitionEvent(const std::string &event, uint8_t transitionAmount) = 0;

        // Updates and returns the output buffers.
        // Return values are the same as GetOutputBuffers
        // Buffers become invalid after the next call to Update().
        virtual const OutputBuffer* Update(int& numOutputBuffers) = 0;

        // Returns an array of buffers with the latest audio data.
        // Buffers become invalid after the next call to Update().
        virtual const OutputBuffer* GetOutputBuffers(int& numOutputBuffers) const = 0;
    };

    IPGTA* CreatePGTA();

    void FreePGTA(IPGTA* pgta);
}
