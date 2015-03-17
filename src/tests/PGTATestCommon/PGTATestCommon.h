
#pragma once

#include <atomic>
#include <string>
#include <cstdint>

enum class PlaybackControl: std::uint32_t
{
    Stop = 0,
    Pause,
    Play,
    NumPlaybackControls
};

namespace PGTATestCommon
{
    void PlayTrack(const std::string trackName, const std::atomic<PlaybackControl>& playbackControl,
                   const std::atomic<uint8_t>& volumeMultiplier, std::string& errorMessage);
}
