
#pragma once

#include <random>
#include <cstdint>
#include <vector>

class PGTASchedulerRNG
{
public:
    PGTASchedulerRNG();
    ~PGTASchedulerRNG();

    bool CanPlay(const float probability);
    float GetDeviation(const float maxDeviation);
    void ShuffleSchedulerOrder(std::vector<uint16_t>& scheduleOrder);
private:
    std::uniform_real_distribution<float> m_probabilityDistribution;
    std::uniform_real_distribution<float> m_deviationDistribution;
    std::mt19937 m_generator;
};
