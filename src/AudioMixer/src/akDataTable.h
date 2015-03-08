
#pragma once

#include <unordered_map>
#include <vector>
#include <assert.h>
#include <stdint.h>

template<typename T>
class DataTable
{
    struct ElemType
    {
        T data;
        uint_fast32_t id;
    };
public:
    // Add a mapping of id to data and
    // store the data by value.
    void AddData(uint_fast32_t id, T* data)
    {
        assert(!HasData(id));

        // get the index where the data will be
        uint_fast32_t index = static_cast<uint_fast32_t>(m_data.size());

        // add the data
        m_data.emplace_back(ElemType{ std::move(*data), id });

        // map the id to an array index
        m_id_to_data.emplace(id, index);
    }

    // Version of AddData that does
    // not modify data
    void AddData(uint_fast32_t id, const T* data)
    {
        T temp(*data);
        AddData(id, &temp);
    }

    // Remove the mapping of id to data.
    void RemoveData(uint_fast32_t id)
    {
        assert(HasData(id));

        const uint_fast32_t removal_index = m_id_to_data[id];
        const uint_fast32_t replacement_index = static_cast<uint_fast32_t>(m_data.size() - 1);

        if (replacement_index > removal_index)
        {
            ElemType& replacementData = m_data[replacement_index];

            // reset the id to data mapping
            m_id_to_data[replacementData.id] = removal_index;

            // assign replacement data to removed data
            m_data[removal_index] = std::move(replacementData);
        }
        else
        {
            assert(replacement_index == removal_index);
        }

        // erase the mapping and remove the data
        m_id_to_data.erase(id);
        m_data.pop_back();
    }
    
    // Returns true if the passed in entity has a
    // component of type T.
    bool HasData(uint_fast32_t id) const
    {
        return (m_id_to_data.count(id) > 0);
    }

    // Returns a const pointer to the component
    // belonging to the passed in entity.
    T* GetData(uint_fast32_t id)
    {
        assert(HasData(id));
        return &m_data[m_id_to_data[id]].data;
    }

    const T* PeekData(uint_fast32_t id)
    {
        assert(HasData(id));
        return &m_data[m_id_to_data[id]].data;
    }

    // If id data exists, updates the internal data,
    // otherwise, adds the new data with the id
    void UpdateOrAddData(uint_fast32_t id, T* data)
    {
        auto it = m_id_to_data.find(id);
        if (it != m_id_to_data.end())
        {
            m_data[it->second] = std::move(*data);
        }
        else
        {
            AddData(id, data);
        }
    }

    // Version of UpdateOrAddData that does not
    // modify data
    void UpdateOrAddData(uint_fast32_t id, const T* data)
    {
        T temp(*data);
        UpdateOrAddData(id, &temp);
    }

    template<typename F>
    void ForEach(F func)
    {
        std::size_t numElems = m_data.size();
        ElemType* data = m_data.data();
        for (uint_fast32_t i = 0; i < numElems;)
        {
            ElemType& thisData = data[i];
            if (!func(thisData.data, thisData.id))
            {
                // TODO create remove from index func
                RemoveData(thisData.id);
                --numElems;
            }
            else
            {
                ++i;
            }
        }
    }

    void Clear()
    {
        m_data.clear();
        m_id_to_data.clear();
    }

private:
    std::vector<ElemType> m_data;
    std::unordered_map<uint_fast32_t, uint_fast32_t> m_id_to_data;
};
