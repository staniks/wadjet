#pragma once

#include <type_traits>

namespace wadjet {

#define WADJET_UNDERLYING(x, y) static_cast<std::underlying_type_t<x>>(y)

#define WADJET_BITMASK(Bitmask)                                       \
                                                                      \
    inline Bitmask operator|(Bitmask x, Bitmask y)                    \
    {                                                                 \
        return static_cast<Bitmask>(WADJET_UNDERLYING(Bitmask, x)     \
                                    | WADJET_UNDERLYING(Bitmask, y)); \
    }                                                                 \
                                                                      \
    inline Bitmask operator&(Bitmask x, Bitmask y)                    \
    {                                                                 \
        return static_cast<Bitmask>(WADJET_UNDERLYING(Bitmask, x)     \
                                    & WADJET_UNDERLYING(Bitmask, y)); \
    }                                                                 \
                                                                      \
    inline Bitmask operator^(Bitmask x, Bitmask y)                    \
    {                                                                 \
        return static_cast<Bitmask>(WADJET_UNDERLYING(Bitmask, x)     \
                                    ^ WADJET_UNDERLYING(Bitmask, y)); \
    }                                                                 \
                                                                      \
    inline Bitmask operator~(Bitmask x)                               \
    {                                                                 \
        return static_cast<Bitmask>(~WADJET_UNDERLYING(Bitmask, x));  \
    }                                                                 \
                                                                      \
    inline Bitmask& operator&=(Bitmask& x, Bitmask y)                 \
    {                                                                 \
        x = x & y;                                                    \
        return x;                                                     \
    }                                                                 \
                                                                      \
    inline Bitmask& operator|=(Bitmask& x, Bitmask y)                 \
    {                                                                 \
        x = x | y;                                                    \
        return x;                                                     \
    }                                                                 \
                                                                      \
    inline Bitmask& operator^=(Bitmask& x, Bitmask y)                 \
    {                                                                 \
        x = x ^ y;                                                    \
        return x;                                                     \
    }

} // namespace wadjet