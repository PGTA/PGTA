
#pragma once

#include <stdint.h>
#include <string.h>
#include <memory.h>

struct PGTAUUID
{
    bool operator==(const PGTAUUID& other) const
    {
        return !memcmp(bytes, other.bytes, sizeof(bytes));
    }

    bool operator!=(const PGTAUUID& other) const
    {
        return !(*this == other);
    }

    PGTAUUID& operator=(const PGTAUUID& other)
    {
        if (this == &other) 
        { 
            return *this;
        }

        memcpy(bytes, other.bytes, sizeof(bytes));
        return *this;
    }

    static const int UUID_NUM_BYTES = 16;
    int8_t bytes[UUID_NUM_BYTES];
};
