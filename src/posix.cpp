#include <wadjet/detail/posix.hpp>

namespace wadjet {
namespace detail {

#ifdef WIN32
int get_socket_api_error() noexcept
{
    return WSAGetLastError();
}
#else
int get_socket_api_error() noexcept
{
    return errno;
}
#endif

} // namespace detail
} // namespace wadjet