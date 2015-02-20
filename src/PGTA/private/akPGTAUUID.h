
#pragma once

#include <stdint.h>
#include <string.h>
#include <memory>

#define NUM_UUID_BYTES 16

struct PGTAUUID
{
    bool operator==(const PGTAUUID& other) const
    {
        return memcmp(bytes, other.bytes, sizeof(bytes)) == 0 ? true : false;
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

    int8_t bytes[NUM_UUID_BYTES];
};
