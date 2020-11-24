#include <wadjet/network.hpp>

#include <wadjet/detail/posix.hpp>

#include <cstring>

namespace wadjet {

socket_address::socket_address(uint32_t address, uint16_t port) noexcept :
    port_m(htons(port)), protocol_m(socket_protocol::ipv4)
{
    ipv4_m.padding_zeroes_1 = 0;
    ipv4_m.padding_zeroes_2 = 0;
    ipv4_m.ffff             = 0xffff;
    ipv4_m.ip               = htonl(address);
}

socket_address::socket_address(std::span<const uint8_t> address, uint16_t port) noexcept :
    port_m(htons(port)), protocol_m(socket_protocol::ipv6)
{
    assert(address.size() == ipv6_size);
    std::memcpy(ipv6_m, address.data(), address.size());
}

socket_address::socket_address(socket_protocol protocol, const char* address_string) :
    socket_address(protocol, address_string, 0)
{
}

socket_address::socket_address(socket_protocol protocol,
                               const char*     address_string,
                               uint16_t        port) :
    port_m(htons(port)), protocol_m(socket_protocol::ipv6)
{
    if(protocol == socket_protocol::ipv6)
    {
        if(::inet_pton(AF_INET6, address_string, (void*)&ipv6_m) != 1)
            throw exception{error_code::socket_address_conversion_fail,
                            detail::get_socket_api_error()};
    }
    else
    {
        ipv4_m.padding_zeroes_1 = 0;
        ipv4_m.padding_zeroes_2 = 0;
        ipv4_m.ffff             = 0xffff;

        if(::inet_pton(AF_INET, address_string, (void*)&ipv4_m.ip) != 1)
            throw exception{error_code::socket_address_conversion_fail,
                            detail::get_socket_api_error()};
    }
}

expected<socket_address, error> socket_address::from_string(socket_protocol protocol,
                                                            const char*     address) noexcept
{
    return from_string(protocol, address, 0);
}

expected<socket_address, error> socket_address::from_string(socket_protocol protocol,
                                                            const char*     address_string,
                                                            uint16_t        port) noexcept
{
    if(protocol == socket_protocol::ipv6)
    {
        uint8_t address[ipv6_size];
        if(::inet_pton(AF_INET6, address_string, (void*)&address) != 1)
            return make_unexpected<error>(error_code::socket_address_conversion_fail,
                                          detail::get_socket_api_error());

        // This is needed since inet_pton fills the buffer in network order.
        return socket_address{std::span{address, sizeof(address)}, port};
    }
    else
    {
        uint32_t network_order_address;
        if(::inet_pton(AF_INET, address_string, (void*)&network_order_address) != 1)
            return make_unexpected<error>(error_code::socket_address_conversion_fail,
                                          detail::get_socket_api_error());

        // This is needed since inet_pton fills the buffer in network order.
        return socket_address{ntohl(network_order_address), port};
    }
}

socket_address socket_address::any(socket_protocol protocol, uint16_t port) noexcept
{
    if(protocol == socket_protocol::ipv6)
    {
        return socket_address{std::span{(uint8_t*)&in6addr_any, sizeof(in6addr_any)}, port};
    }
    else
    {
        return socket_address{(uint32_t)INADDR_ANY, port};
    }
}

expected<std::string_view, error> socket_address::to_string(std::span<char> buffer) const noexcept
{
    socklen_t buffer_length = buffer.size();

    if(protocol_m == socket_protocol::ipv6)
    {
        if(!inet_ntop(AF_INET6, ipv6_m, buffer.data(), buffer_length))
        {
            return make_unexpected<error>(error_code::socket_address_conversion_fail,
                                          detail::get_socket_api_error());
        }
    }
    else
    {
        if(!inet_ntop(AF_INET, &ipv4_m.ip, buffer.data(), buffer_length))
        {
            return make_unexpected<error>(error_code::socket_address_conversion_fail,
                                          detail::get_socket_api_error());
        }
    }

    // On success, inet_ntop will return a null-terminated string, so this should be safe.
    return std::string_view{buffer.data()};
}

socket_address socket_address::any(socket_protocol protocol) noexcept
{
    return any(protocol, 0);
}

socket_address socket_address::loopback(socket_protocol protocol, uint16_t port) noexcept
{
    if(protocol == socket_protocol::ipv6)
    {
        return socket_address{std::span{(uint8_t*)&in6addr_loopback, sizeof(in6addr_loopback)},
                              port};
    }
    else
    {
        return socket_address{(uint32_t)INADDR_LOOPBACK, port};
    }
}

socket_address socket_address::loopback(socket_protocol protocol) noexcept
{
    return loopback(protocol, 0);
}

uint32_t socket_address::ipv4() const noexcept
{
    return ipv4_m.ip;
}

uint16_t socket_address::port_host_order() const noexcept
{
    return ::ntohs(port_m);
}

uint16_t socket_address::port_network_order() const noexcept
{
    return port_m;
}

std::span<const uint8_t> socket_address::ipv6() const noexcept
{
    return std::span{ipv6_m, ipv6_size};
}

} // namespace wadjet