
#pragma once

#include <random>

bool evalProbability( uint32_t probability )
{   
    if (probability == 0)
    {
        return false;
    }
    else if (probability == std::numeric_limits<uint32_t>::max())
    {
        return true;
    }
    
    static std::default_random_engine generator;
    static std::uniform_int_distribution<uint32_t> distribution;
    
    uint32_t value = distribution(generator);

    return probability >= value;
}

