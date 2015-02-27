#include <public/akPGTAUUID.h>
#include <string.h>

int IsUUIDEqualTo(const PGTAUUID* const first, const PGTAUUID* const second)
{
    return !memcmp(first->bytes, second->bytes, sizeof(first->bytes));
}

void SetUUID(PGTAUUID* const first, const PGTAUUID* const second)
{
    if (first == second)
    {
        return;
    }

    memcpy(first->bytes, second->bytes, sizeof(first->bytes));
}