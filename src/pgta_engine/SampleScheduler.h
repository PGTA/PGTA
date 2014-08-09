
#pragma once

#include <memory>
#include <vector>
#include <cstdint>

class EngineTrack;
class AudioStreamBuffer;

class SampleScheduler
{
public:
    SampleScheduler():
        m_engineTrack(nullptr)
    {
    }

    SampleScheduler(EngineTrack *track,
                    std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers);

    void Update();

private:
    bool IsGroupPlaying()
    {
        return true;
    }

private:
    EngineTrack *m_engineTrack;
    std::vector<std::unique_ptr<AudioStreamBuffer>> m_streamBuffers;
    std::vector<uint32_t> m_nextPlayTime;
};
