#pragma once

#include <random>
#include <cstdint>

class PGTASchedulerRNG
{
public:
    PGTASchedulerRNG();
    ~PGTASchedulerRNG();

    bool CanPlay(const float probability);
    float GetDeviation(const float maxDeviation);
    uint16_t SelectFromReadyPool(const uint16_t poolSize);
private:
    std::uniform_real_distribution<float> m_probabilityDistribution;
    std::uniform_real_distribution<float> m_deviationDistribution;
    std::mt19937 m_generator;
};
