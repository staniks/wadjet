#pragma once

#include <wadjet/detail/linking.hpp>
#include <wadjet/flags.hpp>
#include <wadjet/errors.hpp>
#include <wadjet/network.hpp>
#include <wadjet/expected.hpp>

namespace wadjet {

///////////////////////////////////////////////////////////////////////////////////////////////////
// POSIX API RAII wrapper.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Initializes and destroys the underlying socket API if necessary.
class WADJET_DLL socket_api
{
public:
    socket_api();
    ~socket_api();

    // Disable copy - copying the API container might lead to unintended behavior.
    socket_api(const socket_api& other) = delete;
    socket_api& operator=(const socket_api& other) = delete;

    socket_api(socket_api&& other) noexcept;
    socket_api& operator=(socket_api&& other) noexcept;

private:
    // Keep track of whether API was initialized in case of std::move of the container.
    bool initialized_m;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// IPV4/IPV6 socket.
///////////////////////////////////////////////////////////////////////////////////////////////////

// An UDP socket abstraction.
class WADJET_DLL socket
{
public:
    socket(socket_protocol protocol, socket_flags flags);
    ~socket();

    // Disable copy - copying a socket could lead to unintended behavior.
    socket(const socket& other) = delete;
    socket& operator=(const socket& other) = delete;

    socket(socket&& other) noexcept;
    socket& operator=(socket&& other) noexcept;

    socket_protocol protocol() const noexcept;

    // Binds the socket to the provided address.
    error bind(socket_address address) const noexcept;

    // If the socket is bound, returns the address, or an error otherwise.
    expected<socket_address, error> address() const noexcept;

    // Attempt to send the data from a user-provided buffer. Returns an error in case of failure -
    // for example, it might return error_code::socket_would_block under some circumstances.
    error send(socket_address address, std::span<const char> buffer) const noexcept;

    // Check if there are any packets waiting and process them, copying their data into the
    // user-provided buffer. Returns a wadjet::packet structure which provides a view into the
    // buffer, along with the address which the packet came from. In case of failure, returns an
    // error. If there are no packets waiting, it returns error_code::socket_would_block.
    expected<packet, error> recv(std::span<char> buffer) const noexcept;

private:
    socket_protocol protocol_m;

    // Handle provided by underlying socket API.
    using handle_t = int;

    handle_t handle_m;
};

} // namespace wadjet