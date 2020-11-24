#pragma once

#ifdef WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#endif

namespace wadjet {
namespace detail {

inline constexpr const unsigned int api_invalid_socket = -1;
inline constexpr const unsigned int api_socket_error   = -1;

#ifdef WIN32
inline constexpr const unsigned int api_error_would_block = WSAEWOULDBLOCK;
#else
inline constexpr const unsigned int api_error_would_block = EWOULDBLOCK;
#endif

int get_socket_api_error() noexcept;

} // namespace detail
} // namespace wadjet
