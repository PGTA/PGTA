
#pragma once

#include <vector>
#include <utility>

namespace akUtils
{
    template<typename T>
    auto FastRemove(std::vector<T>& vec, typename std::vector<T>::size_type index)
        -> typename std::vector<T>::size_type
    {
        std::swap(vec[index], vec.back());
        vec.pop_back();
        return (index > 0 ? index - 1 : 0);
    }
}
