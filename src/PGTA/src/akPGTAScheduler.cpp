
#include "akPGTAScheduler.h"
#include "akPGTAConst.h"
#include <akAudioMixer.h>
#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <vector>
#include <cmath>

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
    bool isPrimaryMeasuredInBeats = track->GetIsMeasuredInBeats();
    int numSamples = static_cast<int>(track->GetNumSamples());
    m_primaryNextSchedules.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        uint32_t startTime;
        if (isPrimaryMeasuredInBeats)
        {
            startTime = ConvertBeatsToSamples(samples[i].startTime);
        }
        else
        {
            startTime = ConvertTimeToSamples(samples[i].startTime);
        }
        m_primaryNextSchedules[i].samplesUntilPlay = startTime;
        m_primaryNextSchedules[i].samplesOffPeriod = startTime;
    }
}

int32_t PGTAScheduler::ConvertTimeToSamples(const float deltaSeconds) const
{
    uint16_t channels = m_config.audioDesc.channels;
    uint32_t samplesPerSecond = m_config.audioDesc.samplesPerSecond;
    return static_cast<int32_t>(round(deltaSeconds * channels * samplesPerSecond));
}

int32_t PGTAScheduler::ConvertBeatsToSamples(const float deltaBeats) const
{
    uint16_t channels = m_config.audioDesc.channels;
    uint32_t samplesPerSecond = m_config.audioDesc.samplesPerSecond;
    return static_cast<int32_t>(round(deltaBeats / (static_cast<float>(m_config.beatsPerMinute) / 60)
        * channels * samplesPerSecond));
}

PGTABuffer PGTAScheduler::MixScheduleRequests(uint32_t deltaSamples, std::vector<MixRequest>& mixRequests)
{
    std::sort(mixRequests.begin(), mixRequests.end());

    uint64_t samplesMixed = 0;
    akAudioMixer::AudioBuffer output = m_mixer->Update(0);
    m_bufferData.resize(static_cast<size_t>(deltaSamples + output.numSamples));
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
            m_bufferData[static_cast<size_t>(sampleIdx + samplesMixed)] = output.samples[sampleIdx];
        }
        samplesMixed += output.numSamples;

        uint16_t idx = mixRequests[reqNum].sampleId;
        bool isPrimary = mixRequests[reqNum].isPrimary;
        float volumeMultiplier = isPrimary ? m_primaryWeight : 1.0f - m_primaryWeight;
        const PGTATrackSample& sample = isPrimary ? m_primaryTrack->GetSamples()->at(idx) :
            m_transTrack->GetSamples()->at(idx);

        float gain = sample.gain;
   
        float linear = std::exp(gain * std::log(10.0f) / 20.0f) * volumeMultiplier;

        gain = 20 * std::log(linear) / std::log(10.0f);

        if (sample.gain <= PGTAConst::MIN_GAIN)
        {
            continue;
        }
        else if (sample.gain > PGTAConst::MAX_GAIN)
        {
            gain = PGTAConst::MAX_GAIN;
        }

        akAudioMixer::AudioSource source;
        source.SetSource(reinterpret_cast<const int16_t*>(sample.audioData), 
            sample.audioDataNumBytes / m_config.audioDesc.bytesPerSample);

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
        m_bufferData[static_cast<size_t>(sampleIdx + samplesMixed)] = output.samples[sampleIdx];
    }

    PGTABuffer outputBuffer;
    outputBuffer.audioDesc = &m_config.audioDesc;
    outputBuffer.numSamples = static_cast<int32_t>(m_bufferData.size());
    outputBuffer.samples = m_bufferData.data();

    return outputBuffer;
}

void PGTAScheduler::SelectSchedulingCandidates(const bool isPrimary, const uint32_t deltaSamples)
{
    if (!isPrimary && !m_transTrack)
    {
        return;
    }

    int numTrackSamples = isPrimary ? m_primaryTrack->GetNumSamples() : m_transTrack->GetNumSamples();
    bool isMeasuredInBeats = isPrimary ? m_primaryTrack->GetIsMeasuredInBeats() : m_transTrack->GetIsMeasuredInBeats();
    const std::vector<PGTATrackSample>* trackSamples = isPrimary ? m_primaryTrack->GetSamples() : m_transTrack->GetSamples();
    uint16_t numPooledSamples = 0;

    std::vector<ScheduleData>& nextSchedules = isPrimary ? m_primaryNextSchedules : m_transNextSchedules;
    float transValue = isPrimary ? m_primaryWeight : 1.0f - m_primaryWeight;

    for (int i = 0; i < numTrackSamples; ++i)
    {
        const PGTATrackSample& sample = (*trackSamples)[i];

        // Sample is a candidate for playing
        ScheduleData& nextSchedule = nextSchedules[i];
        uint32_t playDelay = nextSchedule.samplesUntilPlay;
        if (playDelay < deltaSamples)
        {
            bool canPlay = true;
            
            if (sample.audioData == nullptr)
            {
                canPlay = false;
            }
            else if (sample.period > 0.0f)
            {
                canPlay = m_rng.CanPlay(sample.probability * (1.0f - (transValue * sample.transitionWeight)));
            }

            if (canPlay && sample.period == 0.0f)
            {
                int32_t numDataSamples = static_cast<int32_t>(sample.audioDataNumBytes /
                    m_config.audioDesc.bytesPerSample);

                nextSchedule.samplesOffPeriod += numDataSamples;
                nextSchedule.samplesUntilPlay = numDataSamples + playDelay;
            }
            else
            {
                uint32_t samplesPerPeriod;
                int32_t deviation;
                if (isMeasuredInBeats)
                {
                    samplesPerPeriod = ConvertBeatsToSamples(sample.period);
                    deviation = ConvertBeatsToSamples(m_rng.GetDeviation(sample.periodDeviation));
                }
                else
                {
                    samplesPerPeriod = ConvertTimeToSamples(sample.period);
                    deviation = ConvertTimeToSamples(m_rng.GetDeviation(sample.periodDeviation));
                }

                nextSchedule.samplesOffPeriod += samplesPerPeriod;
                nextSchedule.samplesUntilPlay = nextSchedule.samplesOffPeriod + deviation;
            }

            bool isInGroup = !sample.group.empty();

            if (canPlay && !isInGroup)
            {
                MixRequest mixRequest;
                mixRequest.isPrimary = isPrimary;
                mixRequest.sampleId = sample.id;
                mixRequest.delay = playDelay;
                m_mixRequests.emplace_back(mixRequest);
            }
            else if (canPlay && isInGroup)
            {
                MixRequest pooledRequest;
                pooledRequest.isPrimary = isPrimary;
                pooledRequest.sampleId = sample.id;
                pooledRequest.delay = playDelay;

                m_pooledRequests.emplace_back(std::pair<std::string, MixRequest>(sample.group, pooledRequest));
                m_poolSchedulerOrder.emplace_back(numPooledSamples);
                ++numPooledSamples;
            }
        }

        nextSchedule.samplesUntilPlay -= deltaSamples;
        nextSchedule.samplesOffPeriod -= deltaSamples;
    }
}

bool PGTAScheduler::HasGroupConflict(const std::string& group, const uint32_t delay)
{
    auto iter = std::find_if(m_groupsNextSchedule.begin(), m_groupsNextSchedule.end(),
        [&group](const std::pair<std::string, uint32_t>& p)
    {
        return p.first == group;
    });

    if (iter != m_groupsNextSchedule.end())
    {
        return true;
    }

    // Check if the group ends before the sample would be scheduled to play
    if (iter != m_groupsNextSchedule.end() && iter->second > delay)
    {
        return true;
    }

    return false;
}

bool PGTAScheduler::HasRestrictionConflict(const bool isPrimary, const std::string& group, const uint32_t delay)
{
    if (!isPrimary && !m_transTrack)
    {
        return false;
    }

    auto restrictions = isPrimary ? m_primaryTrack->GetRestrictions()->find(group) :
        m_transTrack->GetRestrictions()->find(group);
    
    if ((isPrimary && restrictions == m_primaryTrack->GetRestrictions()->end()) ||
        (!isPrimary && restrictions == m_transTrack->GetRestrictions()->end()))
    {
        return false;
    }

    for (const std::string& restriction : (restrictions->second))
    {
        auto resIter = std::find_if(m_groupsNextSchedule.begin(), m_groupsNextSchedule.end(),
            [&restriction](const std::pair<std::string, uint32_t>& p)
        {
            return p.first == restriction;
        });

        if (resIter != m_groupsNextSchedule.end())
        {
            return true;
        }

        // Check if the group ends before the sample would be scheduled to play
        if (resIter != m_groupsNextSchedule.end() && resIter->second > delay)
        {
            return true;
        }
    }

    return false;
}

PGTABuffer PGTAScheduler::Update(const float deltaSeconds)
{
    if (!m_primaryTrack)
    {
        return PGTABuffer();
    }

    m_mixRequests.clear();
    m_endingGroups.clear();
    m_pooledRequests.clear();
    m_poolSchedulerOrder.clear();
    
    uint32_t deltaSamples = ConvertTimeToSamples(deltaSeconds);

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

    /* Check which samples are valid candidates to play and update next schedule times
       Will create mix requests for valid candidates that are loops or are not part of a group
       Will create pooled requests for samples that are valid but in a group (these samples need
       to be further checked for conflicts) */
    SelectSchedulingCandidates(true, deltaSamples);
    SelectSchedulingCandidates(false, deltaSamples);

    // Shuffle the scheduler order to avoid sample play starvation
    m_rng.ShuffleScheduleOrder(m_poolSchedulerOrder);

    for (uint16_t index : m_poolSchedulerOrder)
    {
        std::pair<const std::string, MixRequest>& pooledRequest = m_pooledRequests[index];
        const std::string& group = pooledRequest.first;

        // Check if the group is already playing
        if (HasGroupConflict(group, pooledRequest.second.delay))
        {
            continue;
        }

        // Check if there are any group conflicts due to restrictions
        if (HasRestrictionConflict(pooledRequest.second.isPrimary, group, pooledRequest.second.delay))
        {
            continue;
        }

        // Add the mix request for the sample
        m_mixRequests.emplace_back(pooledRequest.second);

        const std::vector<PGTATrackSample>* trackSamples = pooledRequest.second.isPrimary ? 
            m_primaryTrack->GetSamples() : m_transTrack->GetSamples();

        int32_t numDataSamples = static_cast<int32_t>((*trackSamples)[pooledRequest.second.sampleId].audioDataNumBytes /
            m_config.audioDesc.bytesPerSample);

        uint32_t samplesUntilGroupEnds = numDataSamples + pooledRequest.second.delay - deltaSamples;
        m_groupsNextSchedule.emplace_back(std::pair<const std::string, uint32_t>(m_pooledRequests[index].first, samplesUntilGroupEnds));
    }

    return MixScheduleRequests(deltaSamples, m_mixRequests);
}
