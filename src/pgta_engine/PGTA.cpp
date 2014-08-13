
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

    int numBuffers = config.numBuffers;
    int bufferSize = config.bufferSizeInSamples * config.audioDesc.bytesPerSample;
    m_mixBuffers.clear();
    for (int i = 0; i < numBuffers; ++i)
    {
        m_mixBuffers.emplace_back(new char[bufferSize]);
        m_freeBuffers.emplace(i);
    }
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
    m_mixTime = std::chrono::high_resolution_clock::now();
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
    int numBuffers = m_config.numBuffers;
    m_freeBuffers = std::queue<int>();
    for (int i = 0; i < numBuffers; ++i)
    {
        m_freeBuffers.emplace(i);
    }
    m_outputBuffers.clear();
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    while (now > m_mixTime && !m_freeBuffers.empty())
    {
        //std::cout << m_mixTime.time_since_epoch().count() << std::endl;

        int numBufferSamples = m_config.bufferSizeInSamples;
        auto mixAmount = NumSamplesToDuration(numBufferSamples);
        m_mixTime += mixAmount;

        m_scheduler.Update(mixAmount);

        int buffer = m_freeBuffers.front();
        m_freeBuffers.pop();
        int bufferSize = numBufferSamples * m_config.audioDesc.bytesPerSample;
        int numSamples = m_mixer.Mix(m_mixBuffers[buffer].get(), numBufferSamples, bufferSize);

        PGTA::OutputBuffer output;
        output.audioDesc = &m_config.audioDesc;
        output.audioData = m_mixBuffers[buffer].get();
        output.numSamples = numSamples;
        m_outputBuffers.emplace_back(output);
    }
}

const PGTA::OutputBuffer* PGTAEngine::GetOutputBuffers(int& numBuffers) const
{
    numBuffers = static_cast<int>(m_outputBuffers.size());
    return m_outputBuffers.data();
}

PGTAEngine::TimeDuration PGTAEngine::NumSamplesToDuration(int numSamples)
{ 
    using namespace std::chrono;
    double seconds = static_cast<double>(numSamples) / m_config.audioDesc.samplesPerSecond;
    return duration_cast<TimeDuration>(duration<double>(seconds));
}
