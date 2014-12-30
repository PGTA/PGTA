
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

PGTAEngine::PGTAEngine() : m_isPlaying(false)
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

PGTA::PGTATrackHandle const PGTAEngine::LoadTrack(const std::string &trackName)
{
    // If the track has already been loaded, just return its instance number
    for (const auto& track : m_tracks)
    {
        if (trackName == track->getName())
        {
            return PGTA::PGTATrackHandle(track->getInstance());
        }
    }

    auto track = Initializer::InitializeTrack(trackName.c_str());
    if (!track)
    {
        return PGTA::PGTATrackHandle (-1); // error code
    }

    m_tracks.emplace_back(track);
    return PGTA::PGTATrackHandle(track->getInstance());
}

void PGTAEngine::FreeTrack(const PGTA::PGTATrackHandle handle)
{
    //TODO: should we just delete a track even if it is currently playing?
    auto track = m_tracks.begin();
    while (track != m_tracks.end())
    {
        if ((*track)->getInstance() == handle.instanceNumber)
        {
            delete(*track);
            track = m_tracks.erase(track);
            break; // The instance number should be unique
        }
        else
        {
            ++track;
        }
    }
}

bool PGTAEngine::PlayTrack(const PGTA::PGTATrackHandle  handle)
{

    EngineTrack* track;
    auto trackIter = m_tracks.begin();
    for (trackIter; trackIter != m_tracks.end(); ++trackIter)
    {
        if ((*trackIter)->getInstance() == handle.instanceNumber)
        {
            track = *trackIter;
            break;
        }
    }

    if (trackIter == m_tracks.end())
    {
        return false;
    }

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

    StartPlayback();
    return true;
}

void PGTAEngine::StartPlayback()
{
    m_isPlaying = true;
}

void PGTAEngine::StopPlayback()
{
    m_isPlaying = false;
}

void PGTAEngine::TransitionEvent(const std::string &event, uint8_t transitionAmount)
{
}

const PGTA::OutputBuffer* PGTAEngine::Update(int& numOutputBuffers)
{
    //TODO: check the isPlaying flag to see if you should actually update the buffers
    const int numBuffers = m_config.numBuffers;
    m_freeBuffers = std::queue<int>();
    for (int i = 0; i < numBuffers; ++i)
    {
        m_freeBuffers.emplace(i);
    }
    m_outputBuffers.clear();
    using namespace std::chrono;
    const auto now = high_resolution_clock::now();
    while (now > m_mixTime && !m_freeBuffers.empty())
    {
        //std::cout << m_mixTime.time_since_epoch().count() << std::endl;

        const int numBufferSamples = m_config.bufferSizeInSamples;
        const auto mixAmount = NumSamplesToDuration(numBufferSamples);
        m_mixTime += mixAmount;

        m_scheduler.Update(mixAmount);

        const int buffer = m_freeBuffers.front();
        m_freeBuffers.pop();
        const int bufferSize = numBufferSamples * m_config.audioDesc.bytesPerSample;
        const int numSamples = m_mixer.Mix(m_mixBuffers[buffer].get(), numBufferSamples, bufferSize);

        PGTA::OutputBuffer output;
        output.audioDesc = &m_config.audioDesc;
        output.audioData = m_mixBuffers[buffer].get();
        output.numSamples = numSamples;
        m_outputBuffers.emplace_back(output);
    }

    numOutputBuffers = static_cast<int>(m_outputBuffers.size());
    return m_outputBuffers.data();
}

const PGTA::OutputBuffer* PGTAEngine::GetOutputBuffers(int& numOutputBuffers) const
{
    numOutputBuffers = static_cast<int>(m_outputBuffers.size());
    return m_outputBuffers.data();
}

PGTAEngine::TimeDuration PGTAEngine::NumSamplesToDuration(int numSamples)
{ 
    using namespace std::chrono;
    double seconds = static_cast<double>(numSamples) / m_config.audioDesc.samplesPerSecond;
    return duration_cast<TimeDuration>(duration<double>(seconds));
}