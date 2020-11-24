#include <wadjet/socket.hpp>

#include <wadjet/detail/posix.hpp>

#include <cstring>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Socket API wrapper implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

socket_api::socket_api() : initialized_m(true)
{
#ifdef WIN32
    WSADATA wsa_data;
    if(WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
        throw exception{error_code::socket_api_initialization_fail, detail::get_socket_api_error()};
#endif
}

socket_api::~socket_api()
{
#ifdef WIN32
    if(initialized_m)
        (void)WSACleanup();
#endif
}

socket_api::socket_api(socket_api&& other) noexcept : initialized_m(other.initialized_m)
{
    other.initialized_m = false;
}

socket_api& socket_api::operator=(socket_api&& other) noexcept
{
    initialized_m       = other.initialized_m;
    other.initialized_m = false;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Socket implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

socket::socket(socket_protocol protocol, socket_flags flags) :
    protocol_m(protocol),
    handle_m(
        ::socket(protocol == socket_protocol::ipv6 ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP))
{
    if(handle_m == detail::api_invalid_socket)
    {
        throw exception{error_code::socket_creation_fail, detail::get_socket_api_error()};
    }

    if(protocol == socket_protocol::ipv6 && detail::enum_get(flags, socket_flags::dual_stack))
    {
        int enable = 0;
        if(setsockopt(handle_m, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&enable, sizeof(enable))
           == detail::api_socket_error)
        {
            throw exception{error_code::socket_dual_stack_unavailable,
                            detail::get_socket_api_error()};
        }
    }

#ifdef WIN32
    unsigned long mode = 1;
    if(::ioctlsocket(handle_m, FIONBIO, (unsigned long*)&mode) == detail::api_socket_error)
        throw exception{error_code::socket_mode_fail, detail::get_socket_api_error()};
#else
    int native_flags = fcntl(handle_m, F_GETFL);
    if(native_flags == detail::api_socket_error)
        throw exception{error_code::socket_mode_fail, detail::get_socket_api_error()};

    native_flags |= O_NONBLOCK;

    if(fcntl(handle_m, F_SETFL, native_flags) == detail::api_socket_error)
        throw exception{error_code::socket_mode_fail, detail::get_socket_api_error()};
#endif
}

socket::~socket()
{
    if(handle_m != detail::api_invalid_socket)
#ifdef WIN32
        (void)closesocket(handle_m);
#else
        (void)close(handle_m);
#endif
}

socket::socket(socket&& other) noexcept : handle_m(other.handle_m), protocol_m(other.protocol_m)
{
    other.handle_m = detail::api_invalid_socket;
}

socket& socket::operator=(socket&& other) noexcept
{
    handle_m       = other.handle_m;
    other.handle_m = detail::api_invalid_socket;
    protocol_m     = other.protocol_m;
    return *this;
}

socket_protocol socket::protocol() const noexcept
{
    return protocol_m;
}

error socket::bind(socket_address address) const noexcept
{
    union
    {
        ::sockaddr_in  address_ipv4;
        ::sockaddr_in6 address_ipv6;
    };

    sockaddr* native_address;
    socklen_t native_address_length;
    if(protocol_m == socket_protocol::ipv6)
    {
        native_address        = (sockaddr*)&address_ipv6;
        native_address_length = sizeof(address_ipv6);

        address_ipv6             = {};
        address_ipv6.sin6_family = AF_INET6;
        address_ipv6.sin6_port   = address.port_network_order();

        std::memcpy(&address_ipv6.sin6_addr, address.ipv6().data(), address.ipv6().size());
    }
    else
    {
        native_address        = (sockaddr*)&address_ipv4;
        native_address_length = sizeof(address_ipv4);

        address_ipv4                 = {};
        address_ipv4.sin_family      = AF_INET;
        address_ipv4.sin_addr.s_addr = address.ipv4();
        address_ipv4.sin_port        = address.port_network_order();
    }

    if(::bind(handle_m, native_address, native_address_length) == detail::api_socket_error)
    {
        return error{error_code::socket_bind_error, detail::get_socket_api_error()};
    }

    return error::success();
}

expected<socket_address, error> socket::address() const noexcept
{
    union
    {
        ::sockaddr_in  address_ipv4;
        ::sockaddr_in6 address_ipv6;
    };

    sockaddr* address;
    socklen_t address_length;
    if(protocol_m == socket_protocol::ipv6)
    {
        address        = (sockaddr*)&address_ipv6;
        address_length = sizeof(address_ipv6);
    }
    else
    {
        address        = (sockaddr*)&address_ipv4;
        address_length = sizeof(address_ipv4);
    }

    if(::getsockname(handle_m, address, &address_length) == detail::api_socket_error)
    {
        return make_unexpected<error>(error_code::socket_address_query_fail,
                                      detail::get_socket_api_error());
    }

    if(protocol_m == socket_protocol::ipv6)
    {
        // Port in host byte order.
        const uint16_t port = ntohs(((sockaddr_in6*)address)->sin6_port);

        return socket_address{
            std::span{(uint8_t*)&address_ipv6.sin6_addr, sizeof(address_ipv6.sin6_addr)},
            port};
    }
    else
    {
        // Address in host byte order.
        const uint32_t addr = ntohl(address_ipv4.sin_addr.s_addr);

        // Port in host byte order.
        const uint16_t port = ntohs(((sockaddr_in*)address)->sin_port);

        return socket_address{addr, port};
    }
}

error socket::send(socket_address destination, std::span<const char> buffer) const noexcept
{
    union
    {
        ::sockaddr_in  address_ipv4;
        ::sockaddr_in6 address_ipv6;
    };

    sockaddr* address;
    socklen_t address_length;
    if(protocol_m == socket_protocol::ipv6)
    {
        address        = (sockaddr*)&address_ipv6;
        address_length = sizeof(address_ipv6);

        address_ipv6             = {};
        address_ipv6.sin6_family = AF_INET6;
        address_ipv6.sin6_port   = destination.port_network_order();

        std::memcpy(&address_ipv6.sin6_addr, destination.ipv6().data(), destination.ipv6().size());
    }
    else
    {
        address        = (sockaddr*)&address_ipv4;
        address_length = sizeof(address_ipv4);

        address_ipv4                 = {};
        address_ipv4.sin_family      = AF_INET;
        address_ipv4.sin_port        = destination.port_network_order();
        address_ipv4.sin_addr.s_addr = destination.ipv4();
    }

    if(::sendto(handle_m, buffer.data(), buffer.size(), 0, (const sockaddr*)address, address_length)
       == detail::api_socket_error)
    {
        return error{error_code::socket_send_error, detail::get_socket_api_error()};
    }

    return error::success();
}

expected<packet, error> socket::recv(std::span<char> buffer) const noexcept
{
    union
    {
        ::sockaddr_in  address_ipv4;
        ::sockaddr_in6 address_ipv6;
    };

    sockaddr* address;
    socklen_t address_length;
    if(protocol_m == socket_protocol::ipv6)
    {
        address        = (sockaddr*)&address_ipv6;
        address_length = sizeof(address_ipv6);
    }
    else
    {
        address        = (sockaddr*)&address_ipv4;
        address_length = sizeof(address_ipv4);
    }

    int return_value =
        recvfrom(handle_m, (char*)buffer.data(), buffer.size(), 0, address, &address_length);

    if(return_value >= 0)
    {
        socket_address incoming_address =
            protocol_m == socket_protocol::ipv6 ?
                socket_address{
                    std::span{(uint8_t*)&address_ipv6.sin6_addr, sizeof(address_ipv6.sin6_addr)},
                    ntohs(address_ipv6.sin6_port)} :
                socket_address{ntohl(address_ipv4.sin_addr.s_addr), ntohs(address_ipv4.sin_port)};

        const size_t incoming_size = static_cast<size_t>(return_value);

        return packet{incoming_address, std::span<char>{buffer.data(), incoming_size}};
    }
    else
    {
        const auto api_error = detail::get_socket_api_error();
        if(api_error == detail::api_error_would_block)
            return make_unexpected<error>(error_code::socket_would_block, api_error);

        return make_unexpected<error>(error_code::socket_recv_error, api_error);
    }
}

} // namespace wadjet