
#include "akPGTAScheduler.h"
#include "akPGTAConst.h"
#include <akAudioMixer.h>
#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <vector>
#include <math.h>

PGTAScheduler::PGTAScheduler():
    m_groupsNextSchedule(),
    m_endingGroups(),
    m_primaryTrack(nullptr),
    m_primaryWeight(1.0f),
    m_primaryNextSchedules(),
    m_transTrack(nullptr),
    m_transNextSchedules(),
    m_rng(),
    m_pooledRequests(),
    m_poolSchedulerOrder(),
    m_mixRequests(),
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
        uint32_t startTime = ConvertTimeToSamples(samples[i].startTime);
        m_primaryNextSchedules[i].samplesUntilPlay = startTime;
        m_primaryNextSchedules[i].samplesOffPeriod = startTime;
    }
}

int32_t PGTAScheduler::ConvertTimeToSamples(const float delta)
{
    uint16_t channels = m_config.audioDesc.channels;
    uint32_t samplesPerSecond = m_config.audioDesc.samplesPerSecond;
    return static_cast<int32_t>(round(delta * channels * samplesPerSecond));
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

        uint16_t idx = mixRequests[reqNum].sampleId;
        const PGTATrackSample& sample = m_primaryTrack->GetSamples()->at(idx);
        float gain = sample.volume;

        if (sample.volume <= PGTAConst::MIN_GAIN)
        {
            continue;
        }
        else if (sample.volume > PGTAConst::MAX_GAIN)
        {
            gain = PGTAConst::MAX_GAIN;
        }

        akAudioMixer::AudioSource source;
        source.SetSource(sample.audioData, sample.numSamples);

        akAudioMixer::AudioMixer::MixHandle handle = m_mixer->AddSource(source);
        akAudioMixer::MixControl mixControl = m_mixer->GetMixControl(handle);
        if (mixControl)
        {
            akAudioMixer::MixEffect effect;
            effect.type = akAudioMixer::MixEffects::MixEffect_Gain;
            effect.gain.dBGain = gain;
            mixControl.AddEffect(effect);
        }
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

    m_mixRequests.clear();
    m_endingGroups.clear();
    m_pooledRequests.clear();
    m_poolSchedulerOrder.clear();
    
    uint32_t deltaSamples = ConvertTimeToSamples(delta);

    // Decrement the group schedule times
    for (auto iter = m_groupsNextSchedule.begin(); iter != m_groupsNextSchedule.end();)
    {
        uint32_t remainingSamples = iter->second;
        if (remainingSamples > deltaSamples)
        {
            iter->second -= deltaSamples;
            ++iter;
        }
        else
        {
            m_endingGroups.emplace_back(*iter);
            iter = m_groupsNextSchedule.erase(iter);
        }
    }

    int numTrackSamples = m_primaryTrack->GetNumSamples();
    const std::vector<PGTATrackSample>* samples = m_primaryTrack->GetSamples();
    uint16_t numPooledSamples = 0;

    for (int i = 0; i < numTrackSamples; ++i)
    {
        const PGTATrackSample& sample = (*samples)[i];
        
        // Sample is a candidate for playing
        ScheduleData& primaryNextSchedule = m_primaryNextSchedules[i];
        uint32_t playDelay = primaryNextSchedule.samplesUntilPlay;
        if (playDelay < deltaSamples)
        {
            bool canPlay = m_rng.CanPlay(sample.probability);

            if (sample.audioData == nullptr)
            {
                canPlay = false;
            }
           
            if (sample.period == 0.0f)
            {
                primaryNextSchedule.samplesOffPeriod += sample.numSamples;
                primaryNextSchedule.samplesUntilPlay = sample.numSamples + playDelay;
            }
            else
            {
                uint32_t samplesPerPeriod = ConvertTimeToSamples(sample.period);
                int32_t deviation = ConvertTimeToSamples(m_rng.GetDeviation(sample.periodDeviation));
                primaryNextSchedule.samplesOffPeriod += samplesPerPeriod;
                primaryNextSchedule.samplesUntilPlay = primaryNextSchedule.samplesOffPeriod + deviation;
            }

            bool isInGroup = !sample.group.empty();

            if (canPlay && !isInGroup)
            {
                MixRequest mixRequest;
                mixRequest.sampleId = sample.id;
                mixRequest.delay = playDelay;
                m_mixRequests.emplace_back(mixRequest);
            }
            else if (canPlay && isInGroup)
            {
                MixRequest pooledRequest;
                pooledRequest.sampleId = sample.id;
                pooledRequest.delay = playDelay;

                m_pooledRequests.emplace_back(std::pair<std::string, MixRequest>(sample.group, pooledRequest));
                m_poolSchedulerOrder.emplace_back(numPooledSamples);
                ++numPooledSamples;
            }
        }
       
        m_primaryNextSchedules[i].samplesUntilPlay -= deltaSamples;
        m_primaryNextSchedules[i].samplesOffPeriod -= deltaSamples;

    }

    // Shuffle the scheduler order to avoid sample play starvation
    m_rng.ShuffleSchedulerOrder(m_poolSchedulerOrder);

    for (uint16_t index : m_poolSchedulerOrder)
    {
        bool groupConflict = false;
        std::pair<const std::string, MixRequest>& pooledRequest = m_pooledRequests[index];
        const std::string& group = pooledRequest.first;

        // Check if the group is already playing
        auto iter = std::find_if(m_groupsNextSchedule.begin(), m_groupsNextSchedule.end(),
            [&group](const std::pair<std::string, uint32_t>& p)
        {
            return p.first == group;
        });

        if (iter != m_groupsNextSchedule.end())
        {
            continue;
        }
       
        // Check if the group ends before the sample would be scheduled to play
        if (iter != m_groupsNextSchedule.end() && iter->second > pooledRequest.second.delay)
        {
            continue;
        }

        // Check if there are any group conflicts due to restrictions
        auto restrictions = m_primaryTrack->GetRestrictions()->find(group);
        if (restrictions != m_primaryTrack->GetRestrictions()->end())
        {
            for (const std::string& restriction : (restrictions->second))
            {
                auto resIter = std::find_if(m_groupsNextSchedule.begin(), m_groupsNextSchedule.end(),
                    [&restriction](const std::pair<std::string, uint32_t>& p)
                {
                    return p.first == restriction;
                });

                if (resIter != m_groupsNextSchedule.end())
                {
                    groupConflict = true;
                    break;
                }

                // Check if the group ends before the sample would be scheduled to play
                if (resIter != m_groupsNextSchedule.end() && resIter->second > pooledRequest.second.delay)
                {
                    groupConflict = true;
                    break;
                }
            }
        }

        if (groupConflict)
        {
            continue;
        }

        // Add the mix request for the sample
        m_mixRequests.emplace_back(pooledRequest.second);
        uint32_t samplesUntilGroupEnds = (*m_primaryTrack->GetSamples())[pooledRequest.second.sampleId].numSamples +
            pooledRequest.second.delay - deltaSamples;
        m_groupsNextSchedule.emplace_back(std::pair<const std::string, uint32_t>(m_pooledRequests[index].first, samplesUntilGroupEnds));
    }

    return MixScheduleRequests(deltaSamples, m_mixRequests);
}
