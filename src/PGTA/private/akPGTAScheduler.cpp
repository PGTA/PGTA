#include <private/akPGTAScheduler.h>
#include <akAudioMixer.h>
#include <algorithm>
#include <random>
#include <memory>
#include <math.h>

PGTAScheduler::PGTAScheduler() :
    m_primaryTrack(nullptr),
    m_primaryWeight(1.0f),
    m_primaryNextSchedules(),
    m_transTrack(nullptr),
    m_transNextSchedules(),
    m_groupReadyPools(),
    m_mixRequests(),
    m_rng(),
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

bool MixRequest::operator<(const MixRequest& other) const
{
    return this->delay < other.delay;
}

bool PGTAScheduler::Initialize(const PGTAConfig& config)
{
    m_config = config;

    akAudioMixer::AudioMixerConfig mixerConfig;
    mixerConfig.mixAheadSeconds = config.mixAhead;
    mixerConfig.sampleFramesPerSecond = config.audioDesc.samplesPerSecond;
    if (m_mixer)
    {
        akAudioMixer::FreeAudioMixer(m_mixer);
    }
    
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

    m_transNextSchedules.resize(0);
    m_transTrack = nullptr;

    m_primaryWeight = 1.0f;
    m_primaryTrack = track;

    const PGTATrackSample* samples = track->GetSamples()->data();
    int numSamples = static_cast<int>(track->GetNumSamples());
    m_primaryNextSchedules.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        m_primaryNextSchedules[i] = ConvertTimeToSamples(samples[i].startTime);
    }
}

uint32_t PGTAScheduler::ConvertTimeToSamples(const float delta)
{
    uint16_t channels = m_config.audioDesc.channels;
    uint16_t samplesPerSecond = m_config.audioDesc.samplesPerSecond;
    return static_cast<uint32_t>(round(delta * channels * samplesPerSecond));
}

PGTABuffer PGTAScheduler::MixScheduleRequests(uint32_t deltaSamples, std::vector<MixRequest>& mixRequests)
{
    //Currently only mixes for primary track
    std::sort(mixRequests.begin(), mixRequests.end());

    uint64_t samplesMixed = 0;
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

    if (!m_primaryTrack)
    {
        return PGTABuffer();
    }


    m_mixRequests.resize(0);
    uint32_t deltaSamples = ConvertTimeToSamples(delta);

    // Decrement the group schedule times
    for (std::pair<const std::string, uint32_t>& groupNextSchedule : m_groupsNextShcedule)
    {
        uint32_t remainingSamples = groupNextSchedule.second;
        if (remainingSamples > deltaSamples)
        {
            groupNextSchedule.second -= deltaSamples;
        }
    }

    int numTrackSamples = m_primaryTrack->GetNumSamples();
    const std::vector<PGTATrackSample>* samples = m_primaryTrack->GetSamples();

    for (int i = 0; i < numTrackSamples; ++i)
    {
        const PGTATrackSample& sample = (*samples)[i];
        
        // Sample is a candidate for playing
        if (m_primaryNextSchedules[i] < deltaSamples)
        {
            bool canPlay = m_rng.CanPlay(sample.probability);
            uint32_t delay = m_primaryNextSchedules[i];
           
            if (sample.frequency == 0.0f)
            {
                m_primaryNextSchedules[i] = sample.numSamples + delay;
            }
            else
            {
                m_primaryNextSchedules[i] = ConvertTimeToSamples(sample.frequency) + delay;
            }

            bool groupIsPlaying = false;
            bool isInGroup = !sample.group.empty();
            if (isInGroup)
            {
                std::string group = sample.group;
                std::map<std::string, uint32_t>::iterator iter = m_groupsNextShcedule.find(group);
                if (iter != m_groupsNextShcedule.end() && !(iter->second < delay))
                {
                    groupIsPlaying = true;
                }

                //TODO: Check restrictions before scheduling the sample/group
                if (!groupIsPlaying)
                {
                    std::pair<std::string, uint32_t> groupNextSchedule(group, m_primaryNextSchedules[i]);
                    m_groupsNextShcedule.insert(groupNextSchedule);
                }
            }

            if (canPlay && (!isInGroup || !groupIsPlaying))
            {
                MixRequest mixRequest;
                mixRequest.sampleId = sample.id;
                mixRequest.delay = delay;
                m_mixRequests.emplace_back(mixRequest);
            }
        }
       
        m_primaryNextSchedules[i] -= deltaSamples;

    }

    return MixScheduleRequests(deltaSamples, m_mixRequests);
}
