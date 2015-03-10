#include <private/PGTAScheduler.h>
#include <akAudioMixer.h>
#include <algorithm>
#include <math.h>

PGTAScheduler::PGTAScheduler() :
    m_primaryTrack(nullptr),
    m_primaryWeight(1.0f),
    m_primaryNextSchedules(),
    m_transTrack(nullptr),
    m_transNextSchedules(),
    m_groupReadyPools(),
    m_config(),
    m_mixer(nullptr),
    m_bufferData()
{
}

PGTAScheduler::~PGTAScheduler()
{
    if (m_mixer)
    {
        akAudioMixer::FreeAudioMixer(m_mixer);
    }
}

static bool CompareByDelay(const MixRequest& a, const MixRequest& b)
{
    return a.delay < b.delay;
}

bool PGTAScheduler::Initialize(const PGTAConfig& config)
{
    m_config = config;

    akAudioMixer::AudioMixerConfig mixerConfig;
    mixerConfig.mixAheadSeconds = config.mixAhead;
    mixerConfig.sampleFramesPerSecond = config.audioDesc.samplesPerSecond;
    if (!m_mixer)
    {
        m_mixer = akAudioMixer::CreateAudioMixer(mixerConfig);
    }

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

    m_transNextSchedules.resize(0);
    m_transTrack = nullptr;

    m_primaryWeight = 1.0f;
    m_primaryTrack = track;

    const std::vector<PGTATrackSample> * samples = track->GetSamples();
    int numSamples = static_cast<int>(track->GetNumSamples());
    m_primaryNextSchedules.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        PGTATrackSample sample = (*samples)[i];
        m_primaryNextSchedules[i] = ConvertTimeToSamples(sample.startTime);
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
    std::sort(mixRequests.begin(), mixRequests.end(), CompareByDelay);

    uint32_t samplesMixed = 0;
    akAudioMixer::AudioBuffer output = m_mixer->Update(0);
    m_bufferData.resize(deltaSamples + output.numSamples);
    int numSamples = static_cast<int>(output.numSamples);
    for (int i = 0; i < numSamples; ++i)
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

        numSamples = static_cast<int>(output.numSamples);
        for (int sampleIdx = 0; sampleIdx < numSamples; ++sampleIdx)
        {
            m_bufferData[sampleIdx + samplesMixed] = output.samples[sampleIdx];
        }
        samplesMixed += output.numSamples;

        akAudioMixer::AudioSource source;
        uint16_t idx = mixRequests[reqNum].sampleId;
        PGTATrackSample sample = m_primaryTrack->GetSamples()->at(idx);
        source.SetSource(sample.audioData, sample.numSamples);
        m_mixer->AddSource(source);
    }

    output = m_mixer->Update(deltaSamples - totalDelay);

    numSamples = static_cast<int>(output.numSamples);
    for (int sampleIdx = 0; sampleIdx < numSamples; ++sampleIdx)
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
    const std::vector<PGTATrackSample>* samples = m_primaryTrack->GetSamples();

    for (int i = 0; i < numTrackSamples; ++i)
    {
        const PGTATrackSample& sample = (*samples)[i];
        if (m_primaryNextSchedules[i] < deltaSamples)
        {
            uint32_t delay = m_primaryNextSchedules[i];
            //if (sample.group.empty())
            //{
                if (sample.frequency == 0.0f)
                {
                    m_primaryNextSchedules[i] = sample.numSamples + delay;
                }
                else
                {
                    m_primaryNextSchedules[i] = ConvertTimeToSamples(sample.frequency) + delay;
                }

                mixRequests.emplace_back(MixRequest{ sample.id, delay });
           // }
        }
       
        m_primaryNextSchedules[i] -= deltaSamples;

    }

    return MixScheduleRequests(deltaSamples, mixRequests);
}
