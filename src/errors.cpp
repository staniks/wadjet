#include <wadjet/errors.hpp>

#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Error code descriptions.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace wadjet {
namespace detail {
inline const std::map<error_code, zstring_view> error_descriptions = {
    {error_code::socket_api_initialization_fail, "failed to initialize socket API"},
    {error_code::socket_creation_fail, "failed to create socket"},
    {error_code::socket_dual_stack_unavailable, "failed to create dual-stack IPV6 socket"},
    {error_code::socket_mode_fail, "failed to set socket mode"},
    {error_code::socket_bind_error, "failed to bind socket to specified address"},
    {error_code::socket_address_query_fail, "failed to query socket address"},
    {error_code::socket_send_error, "failed to send data"},
    {error_code::socket_recv_error, "failed to receive data"},
    {error_code::socket_would_block, "no data received at the time"},
    {error_code::socket_address_conversion_fail, "failed to convert string to address"},
};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Error implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

zstring_view error::description() const noexcept
{
    auto it = detail::error_descriptions.find(code);
    if(it != detail::error_descriptions.end())
        return it->second;

    return "unknown error";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Exception implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

exception::exception(wadjet::error error) noexcept : error_m(error)
{
}

exception::exception(wadjet::error_code            code,
                     wadjet::underlying_error_code underlying_code) noexcept :
    error_m(code, underlying_code)
{
}

wadjet::error exception::error() const noexcept
{
    return error_m;
}

const char* exception::what() const noexcept
{
    return error_m.description().data();
}

} // namespace wadjet