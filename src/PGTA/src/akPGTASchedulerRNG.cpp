
#include "akPGTASchedulerRNG.h"
#include <ctime>
#include <random>
#include <algorithm>

PGTASchedulerRNG::PGTASchedulerRNG() :
    m_probabilityDistribution(0.0f, 1.0f),
    m_deviationDistribution(-1.0f, 1.0f),
    m_generator(static_cast<unsigned int>(time(nullptr)))
{
}

PGTASchedulerRNG::~PGTASchedulerRNG()
{
}

float PGTASchedulerRNG::GetDeviation(const float maxDeviation)
{
    return m_deviationDistribution(m_generator) * maxDeviation;
}

bool PGTASchedulerRNG::CanPlay(const float probability)
{
    return probability >= m_probabilityDistribution(m_generator);
}

void PGTASchedulerRNG::ShuffleScheduleOrder(std::vector<uint16_t>& scheduleOrder)
{
    std::shuffle(scheduleOrder.begin(), scheduleOrder.end(), m_generator);
}
