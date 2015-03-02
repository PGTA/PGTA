
#pragma once

#include <stdint.h>

struct PGTAUUID
{
    const static int UUID_NUM_BYTES = 32;
    char bytes[UUID_NUM_BYTES];

    bool operator==(const PGTAUUID& other) const;
    bool operator!=(const PGTAUUID& other) const;
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const;
    PGTAUUID& operator=(const PGTAUUID& other);
    bool operator<(const PGTAUUID& other) const;
};