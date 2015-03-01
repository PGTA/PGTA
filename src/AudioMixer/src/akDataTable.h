
#pragma once

#include <unordered_map>
#include <vector>
#include <assert.h>

template<typename T>
class DataTable
{
public:
    // Add a mapping of id to data and
    // store the data by value.
    void AddData(uint_fast32_t id, T* data)
    {
        assert(!HasData(id));

        // get the index where the data will be
        uint_fast32_t index = static_cast<uint_fast32_t>(m_data.size());

        // add the data
        m_data.emplace_back(std::move(*data));

        // map the id to an array index
        m_id_to_data.emplace(id, index);
        m_data_to_id.emplace(index, id);
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

        uint_fast32_t removal_index = m_id_to_data[id];
        //RemoveDataFromIndex(removal_index);
        uint_fast32_t replacement_index = static_cast<uint_fast32_t>(m_data.size() - 1);

        if (replacement_index > removal_index)
        {
            // assign replacement data to removed data
            m_data[removal_index] = std::move(m_data[replacement_index]);

            uint_fast32_t replacement_id = m_data_to_id[replacement_index];

            // swap the id to data mappings
            m_id_to_data[replacement_id] = removal_index;
            m_data_to_id[removal_index] = replacement_id;
        }
        else
        {
            assert(replacement_index == removal_index);
        }

        // erase the mappings and data
        m_data_to_id.erase(replacement_index);
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
        return &m_data[m_id_to_data[id]];
    }

    const T* PeekData(uint_fast32_t id)
    {
        assert(HasData(id));
        return &m_data[m_id_to_data[id]];
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
        T* data = m_data.data();
        for (uint_fast32_t i = 0; i < numElems;)
        {
            if (!func(data[i]))
            {
                // TODO create remove from index func
                RemoveData(m_data_to_id[i]);
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
        m_data_to_id.clear();
    }

private:
    std::vector<T> m_data;
    std::unordered_map<uint_fast32_t, uint_fast32_t> m_id_to_data;
    std::unordered_map<uint_fast32_t, uint_fast32_t> m_data_to_id;
};
