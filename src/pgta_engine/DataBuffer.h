
#pragma once

#include <cstdint>
#include <deque>
#include <algorithm>
#include <vector>

template<typename T>
class DataBuffer
{
public:
    void PushData(const T* data, uint64_t count)
    {
        m_buffer.insert(m_buffer.end(), data, data+count);
    }
    
    int PullData(T* data, int maxCount)
    {
        auto maxPull = std::min((int)m_buffer.size(), maxCount);
        std::copy(m_buffer.begin(), m_buffer.begin()+maxPull, data);
        m_buffer.erase(m_buffer.begin(), m_buffer.begin()+maxPull);
        return maxPull;
    }
    
    int GetNumBytes() const
    {
        return (int)m_buffer.size() * sizeof(T);
    }
private:
    std::vector<T> m_buffer;
};
