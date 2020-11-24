#pragma once

#include <wadjet/detail/bitmask.hpp>

#include <cstdint>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Socket flags.
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class socket_flags : uint64_t
{
    none       = 0ULL,
    dual_stack = 1ULL
};

WADJET_BITMASK(socket_flags);

namespace detail {
template<typename T>
inline constexpr bool enum_get(T flags, T flag)
{
    return (flags & flag) == flag;
}

template<typename T>
inline constexpr void enum_set(T flags, T flag)
{
    flags |= flag;
}
} // namespace detail

} // namespace wadjet