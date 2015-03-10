#include <private/akPGTASchedulerRNG.h>
#include <random>

PGTASchedulerRNG::PGTASchedulerRNG() :
    m_distribution(0.0f, 1.0f),
    m_generator()
{
}

PGTASchedulerRNG::~PGTASchedulerRNG()
{
}

bool PGTASchedulerRNG::CanPlay(const float probability)
{
    return probability >= m_distribution(m_generator);
}
