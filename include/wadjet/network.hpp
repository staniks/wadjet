#pragma once

#include <wadjet/detail/linking.hpp>

#include <wadjet/expected.hpp>
#include <wadjet/errors.hpp>

#include <span>
#include <cstdint>
#include <cstddef>
#include <string_view>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Socket protocol.
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class socket_protocol
{
    ipv4,
    ipv6
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Socket address.
///////////////////////////////////////////////////////////////////////////////////////////////////

// The socket address wrapper provides an abstraction over socket API address and various utility
// functions for creating an address from string, conversion to string, and similar. Parameters like
// port are expected to be provided in host order for simplicity.
class WADJET_DLL socket_address
{
public:
    inline static constexpr size_t ipv6_size = 16;

    // Create an address from raw IPV4 and port. IPV4 and port are in host order.
    socket_address(uint32_t ipv4, uint16_t port) noexcept;

    // Create an address from raw IPV6 array and port. Port is in host order.
    socket_address(std::span<const uint8_t> ipv6, uint16_t port) noexcept; // TODO: SPAN

    // Create an address from string on specified port. Throws on failure.
    socket_address(socket_protocol protocol, const char* address, uint16_t port);

    // Create an address from string. Port is zero. Throws on failure.
    socket_address(socket_protocol protocol, const char* address);

    // Create an address from string on specified port.
    static expected<socket_address, error>
    from_string(socket_protocol protocol, const char* address, uint16_t port) noexcept;

    // Create an address from string. Port is zero.
    static expected<socket_address, error> from_string(socket_protocol protocol,
                                                       const char*     address) noexcept;

    // Create an address corresponding to all available interfaces, with the specified port.
    static socket_address any(socket_protocol protocol, uint16_t port) noexcept;

    // Create an address corresponding to all available interfaces, with port zero.
    static socket_address any(socket_protocol protocol) noexcept;

    // Create an address corresponding to the loopback interface, with the specified port.
    static socket_address loopback(socket_protocol protocol, uint16_t port) noexcept;

    // Create an address corresponding to the loopback interface, with port zero.
    static socket_address loopback(socket_protocol protocol) noexcept;

    // Attempt to convert the address to a string representation. The user must provide a buffer
    // large enough to store an IPV6 address (64 bytes + 1 byte for null character). Returns a
    // string view into the buffer representing the resulting address, or an error.
    expected<std::string_view, error> to_string(std::span<char> buffer) const noexcept;

    uint16_t port_host_order() const noexcept;
    uint16_t port_network_order() const noexcept;

    std::span<const uint8_t> ipv6() const noexcept;

    // Returns the raw IPV4 in network order.
    uint32_t ipv4() const noexcept;

private:
    // Makes IPV6 <-> IPV4 interoperability easier.
    struct mapped_ipv4
    {
        uint64_t padding_zeroes_1;
        uint16_t padding_zeroes_2;
        uint16_t ffff;
        uint32_t ip;
    };

    union
    {
        // Both IPV6 and IPV4 are kept in network order.
        uint8_t     ipv6_m[ipv6_size];
        mapped_ipv4 ipv4_m;

        static_assert(sizeof(ipv4_m) == sizeof(ipv6_m));
    };

    // Port is kept in network order.
    const uint16_t port_m;

    const socket_protocol protocol_m;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Packet structure.
///////////////////////////////////////////////////////////////////////////////////////////////////

struct WADJET_DLL packet
{
    inline packet(socket_address address, std::span<char> payload) :
        address(address), payload(payload)
    {
    }

    // Address from which the packet came from.
    socket_address address;

    // A view into the user-provided buffer. Represents packet contents.
    std::span<char> payload;
};

} // namespace wadjet
