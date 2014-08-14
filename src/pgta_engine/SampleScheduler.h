
#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include <map>
#include <cstdint>

class EngineTrack;
class AudioStreamBuffer;

class SampleScheduler
{
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using TimeDuration = std::chrono::high_resolution_clock::duration;
public:
    SampleScheduler():
        m_engineTrack(nullptr)
    {
    }

    void Initialize(EngineTrack* track,
                    std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers);

    void Update(TimeDuration dt);

private:
    bool IsGroupPlaying()
    {
        return true;
    }

private:
    EngineTrack *m_engineTrack;
    std::vector<std::unique_ptr<AudioStreamBuffer>> m_streamBuffers;
    std::vector<TimeDuration> m_nextCheckCountdowns;
    std::vector<char> m_paddingBuffer;
    std::vector <TimeDuration> m_groupRemainingDurations;
    std::vector< std::vector<uint16_t> > m_readyGroupSamples;
};
