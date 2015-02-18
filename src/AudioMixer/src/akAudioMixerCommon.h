
#pragma once

#include <assert.h>

template<typename T>
auto ToImpl(T* thisptr) -> typename T::Impl*
{
    auto ptr = static_cast<typename T::Impl*>(thisptr);
    assert(ptr);
    return ptr;
}

template<typename T>
auto ToImpl(const T* thisptr) -> typename T::Impl const*
{
    auto ptr = static_cast<typename T::Impl const*>(thisptr);
    assert(ptr);
    return ptr;
}
