
#include "PGTA.h"

#include "Initializer.h"
#include <chrono>

namespace PGTA
{
    IPGTA* CreatePGTA()
    {
        return new PGTAEngine();
    }

    void FreePGTA(IPGTA* pgta)
    {
        delete pgta;
    }
}

PGTAEngine::PGTAEngine()
{
}

PGTAEngine::~PGTAEngine()
{
}

void PGTAEngine::Initialize(const PGTA::PGTAConfig &config)
{
    m_config = config;
    m_mixerOutput.resize(config.bufferSize);
    m_outputBuffers.resize(1);
}

bool PGTAEngine::StartPlayback(const std::string &trackName)
{
    auto track = Initializer::InitializeTrack(trackName.c_str());
    if (!track)
    {
        return false;
    }
    m_currentTrack.reset(track);

    std::vector<std::unique_ptr<AudioStreamBuffer>> streamBuffers;
    for (auto& sample : track->getSamples())
    {
        const auto *audioSample = sample.getSample();
        auto *streamBuffer = new AudioStreamBuffer(audioSample->getBitsPerSample());
        streamBuffers.emplace_back(streamBuffer);
        m_mixer.ConnectInputStream(streamBuffer);
    }
    m_scheduler.Initialize(track, std::move(streamBuffers));

    m_lastUpdate = std::chrono::high_resolution_clock::now();
    return true;
}

void PGTAEngine::StopPlayback()
{
}

void PGTAEngine::TransitionEvent(const std::string &event, uint8_t transitionAmount)
{
}

void PGTAEngine::Update()
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto dt = now - m_lastUpdate;
    m_lastUpdate = now;
    
    m_scheduler.Update(now);
    int numSamples = m_mixer.Mix(dt, m_mixerOutput);

    m_outputBuffers.clear();
    PGTA::OutputBuffer output;
    output.audioDesc = &m_config.audioDesc;
    output.audioData = m_mixerOutput.data();
    output.numSamples = numSamples;
    m_outputBuffers.emplace_back(output);
}

const PGTA::OutputBuffer* PGTAEngine::GetOutputBuffers(int& numBuffers) const
{
    numBuffers = static_cast<int>(m_outputBuffers.size());
    return m_outputBuffers.data();
}
