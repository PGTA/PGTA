#pragma once

#include <random>

class PGTASchedulerRNG
{
public:
    PGTASchedulerRNG();
    ~PGTASchedulerRNG();

    bool CanPlay(const float probability);

private:
    std::uniform_real_distribution<float> m_distribution;
    std::mt19937 m_generator;
};