
#pragma once

#include <SDL_endian.h>
#include <cstdint>

//
// Written by Sree Kotay at some point in time...
// Real2Int: Returns the floor of a real value
//

// 2^36 * 1.5,  (52-_shiftamt=36) uses limited precisicion to floor
static const double _double2fixmagic = 68719476736.0*1.5;

// 16.16 fixed point representation
static const int32_t _shiftamt = 16;

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    #define iexp_				0
    #define iman_				1
#else
    #define iexp_				1
    #define iman_				0
#endif //BigEndian_

// ================================================================================================
// Real2Int
// ================================================================================================
inline int32_t Real2Int(double val)
{
    val = val + _double2fixmagic;
    return ((int32_t*)&val)[iman_] >> _shiftamt;
}

// ================================================================================================
// Real2Int
// ================================================================================================
inline int32_t Real2Int(float val)
{
    return Real2Int((double)val);
}
