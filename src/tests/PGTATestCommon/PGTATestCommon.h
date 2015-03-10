
#pragma once

#include <atomic>
#include <string>

namespace PGTATestCommon
{
    void PlayTrack(std::string trackName, std::atomic<int> &playbackControl);
}
