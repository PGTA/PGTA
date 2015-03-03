#include <private/akPGTAUUID.h>
#include <string.h>

bool PGTAUUID::operator==(const PGTAUUID& other) const
{
    return !memcmp(bytes, other.bytes, sizeof(bytes));
}

bool PGTAUUID::operator!=(const PGTAUUID& other) const
{
    return !(*this == other);
}

bool PGTAUUID::operator==(const char* other) const
{
    return !memcmp(bytes, other, sizeof(bytes));
}

bool PGTAUUID::operator!=(const char* other) const
{
    return !(*this == other);
}

PGTAUUID& PGTAUUID::operator=(const PGTAUUID& other)
{
    if (this == &other)
    {
        return *this;
    }

    memcpy(bytes, other.bytes, sizeof(bytes));
    return *this;
}

PGTAUUID& PGTAUUID::operator=(const char* other)
{
    memcpy(bytes, other, sizeof(bytes));
    return *this;
}

bool PGTAUUID::operator<(const PGTAUUID& other) const
{
    return memcmp(bytes, other.bytes, sizeof(bytes)) < 0 ? true : false;
}