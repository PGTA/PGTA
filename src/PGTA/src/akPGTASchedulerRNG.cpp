
#include "akPGTASchedulerRNG.h"
#include <ctime>
#include <random>

PGTASchedulerRNG::PGTASchedulerRNG() :
    m_probabilityDistribution(0.0f, 1.0f),
    m_deviationDistribution(-1.0f, 1.0f),
    m_generator(time(nullptr))
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

uint16_t PGTASchedulerRNG::SelectFromReadyPool(const uint16_t poolSize)
{
    return std::uniform_int_distribution<uint16_t>{0, static_cast<uint16_t>(poolSize - 1)}(m_generator);
}
