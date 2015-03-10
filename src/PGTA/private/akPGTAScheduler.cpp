#include <private/PGTAScheduler.h>
#include <akAudioMixer.h>
#include <algorithm>
#include <math.h>

PGTAScheduler::PGTAScheduler() :
    m_primaryWeight(1.0f)
{
}

PGTAScheduler::~PGTAScheduler()
{
}

static bool compareByDelay(const MixRequest& a, const MixRequest& b)
{
    return a.delay < b.delay;
}

bool PGTAScheduler::Initialize(const PGTAConfig& config)
{
    m_config = config;

    akAudioMixer::AudioMixerConfig mixerConfig;
    mixerConfig.mixAheadSeconds = config.mixAhead;
    mixerConfig.sampleFramesPerSecond = config.audioDesc.samplesPerSecond;
    m_mixer = akAudioMixer::CreateAudioMixer(mixerConfig);
    if (!m_mixer)
    {
        return false;
    }

    return true;
}

void PGTAScheduler::SetPrimaryTrack(const PGTATrack* track)
{
    if (!track)
    {
        return;
    }

    m_transNextShedules.resize(0);
    m_transTrack = nullptr;

    m_primaryWeight = 1.0f;
    m_primaryTrack = track;

    auto* samples = track->GetSamples();
    int numSamples = static_cast<int>(track->GetNumSamples());
    m_primaryNextShedules.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        auto sample = (*samples)[i];
        m_primaryNextShedules[i] = ConvertTimeToSamples(sample.startTime);
    }
}

uint32_t PGTAScheduler::ConvertTimeToSamples(float delta)
{
    uint16_t channels = m_config.audioDesc.channels;
    uint16_t samplesPerSecond = m_config.audioDesc.samplesPerSecond;
    return static_cast<uint32_t>(round(delta * channels * samplesPerSecond));
}

PGTABuffer PGTAScheduler::MixScheduleRequests(uint32_t deltaSamples, std::vector<MixRequest>& mixRequests)
{
    //Currently only mixes for primary track
    std::sort(mixRequests.begin(), mixRequests.end(), compareByDelay);

    uint32_t samplesMixed = 0;
    akAudioMixer::AudioBuffer output = m_mixer->Update(0);
    m_bufferData.resize(deltaSamples + output.numSamples);
    for (int i = 0; i < output.numSamples; ++i)
    {
        m_bufferData[i] = output.samples[i];
    }

    samplesMixed = output.numSamples;

    uint32_t totalDelay = 0;
    int numRequests = static_cast<int>(mixRequests.size());
    for (int reqNum = 0; reqNum < numRequests; ++reqNum)
    {
        uint32_t delay = mixRequests[reqNum].delay - totalDelay;
        totalDelay += delay;
        output = m_mixer->Update(delay);

        for (int sampleIdx = 0; sampleIdx < output.numSamples; ++sampleIdx)
        {
            m_bufferData[sampleIdx + samplesMixed] = output.samples[sampleIdx];
        }
        samplesMixed += output.numSamples;

        akAudioMixer::AudioSource source;
        auto idx = mixRequests[reqNum].sampleId;
        PGTATrackSample sample = m_primaryTrack->GetSamples()->at(idx);
        source.SetSource(sample.audioData, sample.numSamples);
        m_mixer->AddSource(source);
    }

    output = m_mixer->Update(deltaSamples - totalDelay);

    for (int sampleIdx = 0; sampleIdx < output.numSamples; ++sampleIdx)
    {
        m_bufferData[sampleIdx + samplesMixed] = output.samples[sampleIdx];
    }

    PGTABuffer outputBuffer;
    outputBuffer.audioDesc = &m_config.audioDesc;
    outputBuffer.numSamples = static_cast<int32_t>(m_bufferData.size());
    outputBuffer.samples = m_bufferData.data();

    return outputBuffer;
}

PGTABuffer PGTAScheduler::Update(const float delta)
{
    // Todo: Handle case where tracks have not been loaded yet

    std::vector<MixRequest> mixRequests;
    uint32_t deltaSamples = ConvertTimeToSamples(delta);

    int numTrackSamples = m_primaryTrack->GetNumSamples();
    auto* samples = m_primaryTrack->GetSamples();

    for (int i = 0; i < numTrackSamples; ++i)
    {
        auto& sample = (*samples)[i];
        if (m_primaryNextShedules[i] < deltaSamples)
        {
            uint32_t delay = m_primaryNextShedules[i];
            if (sample.group.empty())
            {
                if (sample.frequency == 0.0f)
                {
                    m_primaryNextShedules[i] = sample.numSamples + delay;
                }
                else
                {
                    m_primaryNextShedules[i] = ConvertTimeToSamples(sample.frequency) + delay;
                }

                mixRequests.emplace_back(MixRequest{ sample.id, delay });
            }
        }
       
        m_primaryNextShedules[i] -= deltaSamples;

    }

    return MixScheduleRequests(deltaSamples, mixRequests);
}
