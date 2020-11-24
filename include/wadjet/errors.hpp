#pragma once

#include <wadjet/detail/linking.hpp>

#include <cassert>
#include <stdexcept>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Error codes.
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class error_code
{
    none,
    socket_api_initialization_fail,
    socket_creation_fail,
    socket_dual_stack_unavailable,
    socket_mode_fail,
    socket_bind_error,
    socket_address_query_fail,
    socket_send_error,
    socket_recv_error,
    socket_would_block,
    socket_address_conversion_fail
};

// Error code returned from within Winsock or POSIX socket API.
using underlying_error_code = int;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Error structure.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Error structure. Provides access to both high-level wadjet error code and the internal error code
// of the underlying socket API.
struct WADJET_DLL error
{
    constexpr error(error_code code, underlying_error_code underlying_code) noexcept;

    // Creates an error which signals success.
    static constexpr error success() noexcept;

    // Implicit conversion to error_code for quality-of-life.
    constexpr bool operator==(const error_code other_code) const noexcept;
    constexpr bool operator!=(const error_code other_code) const noexcept;

    constexpr error_code operator*() const noexcept;

    // Returns a human-readable error code description.
    const char* description() const noexcept;

    // Wadjet error code.
    const error_code code;

    // Internal socket API error code.
    const underlying_error_code underlying_code;

private:
    // Reserved for instantiating a success object.
    constexpr error(error_code code) noexcept;
};

inline constexpr error::error(error_code code) noexcept : code(code), underlying_code(0)
{
    // This is only allowed for returning error_code::none.
    assert(code == error_code::none);
}

inline constexpr error::error(error_code code, underlying_error_code underlying_code) noexcept :
    code(code), underlying_code(underlying_code)
{
}

inline constexpr error error::success() noexcept
{
    return error{error_code::none};
}

inline constexpr bool error::operator==(const error_code other_code) const noexcept
{
    return code == other_code;
}

inline constexpr bool error::operator!=(const error_code other_code) const noexcept
{
    return code != other_code;
}

inline constexpr error_code error::operator*() const noexcept
{
    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Exception class.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Wadjet exceptions merely serve as wrappers around errors and can be caught with std::exception
// handlers.
class WADJET_DLL exception : public std::exception
{
public:
    exception(wadjet::error error) noexcept;
    exception(wadjet::error_code code, wadjet::underlying_error_code underlying_code) noexcept;

    wadjet::error error() const noexcept;

    const char* what() const noexcept override;

private:
    const wadjet::error error_m;
};

} // namespace wadjet