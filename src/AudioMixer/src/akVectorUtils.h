
#pragma once

#include <vector>
#include <utility>

namespace akUtils
{
    template<typename T>
    void FastRemove(std::vector<T>& vec, typename std::vector<T>::size_type index)
    {
        std::swap(vec[index], vec.back());
        vec.pop_back();
    }
}
