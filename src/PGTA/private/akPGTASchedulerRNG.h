#pragma once

#include <random>

class PGTASchedulerRNG
{
public:
    PGTASchedulerRNG();
    ~PGTASchedulerRNG();

    bool CanPlay(const float probability);
    float GetDeviation(const float maxDeviation);
private:
    std::uniform_real_distribution<float> m_probabilityDistribution;
    std::uniform_real_distribution<float> m_deviationDistribution;
    std::mt19937 m_generator;
};
