
#pragma once

#include <stdint.h>

static const int UUID_NUM_BYTES = 16;

typedef struct PGTAUUID
{
    int8_t bytes[16];
} PGTAUUID;

int IsUUIDEqualTo(const PGTAUUID* const first, const PGTAUUID* const second);

void SetUUID(PGTAUUID* const target, const PGTAUUID* const source);