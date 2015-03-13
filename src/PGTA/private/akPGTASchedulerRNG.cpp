#include <private/akPGTASchedulerRNG.h>
#include <random>

PGTASchedulerRNG::PGTASchedulerRNG() :
    m_probabilityDistribution(0.0f, 1.0f),
    m_deviationDistribution(-1.0f, 1.0f),
    m_generator()
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
