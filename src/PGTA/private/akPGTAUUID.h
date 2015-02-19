
#pragma once

#include <stdint.h>

#define NUM_UUID_BYTES 16

class PGTAUUID
{
public:
    PGTAUUID(int8_t bytes[]);
    ~PGTAUUID();

    bool operator==(const PGTAUUID& other);
    bool operator!=(const PGTAUUID& other);
private:
    PGTAUUID();
    int8_t bytes[NUM_UUID_BYTES];
};
