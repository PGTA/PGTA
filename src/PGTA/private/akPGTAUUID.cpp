#include <public/akPGTAUUID.h>
#include <string.h>

int IsUUIDEqualTo(const PGTAUUID* const first, const PGTAUUID* const second)
{
    return !memcmp(first->bytes, second->bytes, sizeof(first->bytes));
}

void SetUUID(PGTAUUID* const target, const PGTAUUID* const source)
{
    if (target == source)
    {
        return;
    }

    memcpy(target->bytes, source->bytes, sizeof(target->bytes));
}