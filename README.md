# wadjet

A modern, thin wrapper around UDP sockets.

![example workflow](https://github.com/staniks/wadjet/actions/workflows/build.yml/badge.svg)    ![example workflow](https://github.com/staniks/wadjet/actions/workflows/tests.yml/badge.svg)

_See [GitHub Actions tab](https://github.com/staniks/wadjet/actions) for automated builds, tests and artifacts._

## Features

- thin wrapper around Winsock/POSIX UDP sockets
- supports IPV4 and IPV6 sockets (with dual stack)
- stack-based design &mdash; no allocations
- minimal reliance on exceptions &mdash; acquisition only
- error handling inspired by [std::expected standard proposal](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0323r10.html)
- easy access to internal error codes of the underlying socket API

## Overview

### Socket Instantiation

Before working with any sockets, socket API must be initialized. This is needed because explicit API initialization is required on some platforms (e.g. Windows due to Winsock).

`wadjet::socket_api` is a simple guard object which will initialize the underlying socket API on construction, and perform cleanup on destruction &mdash; if this object goes out of scope, most socket operations will fail, so keep it safe!

```C++
// Socket API guard object.
socket_api api;
```

`wadjet` provides simple interface for instantiating UDP sockets:

```C++
// Instantiate a IPV4 socket.
socket socket{socket_protocol::ipv4, socket_flags::none};

// Instantiate a dual-stack IPV6 socket.
socket socket{socket_protocol::ipv6, socket_flags::dual_stack};
```

Exceptions can be thrown on resource acquisition for `socket`, `socket_api` and `socket_address`, so it is recommended to handle them in a manner similar to:

```C++
try
{
    socket socket{socket_protocol::ipv4, socket_flags::none};
}
catch(const wadjet::exception& e)
{
    std::cerr << e.what() << std::endl;
}
```

Exceptions also carry `wadjet::error` objects. These contain wadjet error codes and internal error codes of the underlying socket API, which can be useful for debugging or logging. For example, `wadjet` may return `error_code::socket_would_block`, but its internal error code could be `WSAEWOULDBLOCK` or `EWOULDBLOCK`, depending on the platform.


```C++
// ...in exception handler

const wadjet::error error = e.error();

// Implicit conversion error <-> error_code for usability.
// Alternatively - error.code == error_code::socket_creation_fail
if(error == wadjet::error_code::socket_creation_fail)
{
    // Acquire Winsock/POSIX error code.
    int underlying_code = error.underlying_code;

    // ...perform logging
}
```

### Socket Address

`socket_address` is a wrapper around an IPV4 or an IPV6 address, depending on how it's constructed. IPV4 addresses are internally kept as IPV4-mapped IPV6 addresses.

Alongside the address itself, the `socket_address` structure also contains the UDP port.

The interface allows creating a `socket_address` in a variety of ways, such as:

```C++
// Throws in case of failure.
socket_address address{socket_protocol::ipv4, "127.0.0.1", 8086};

// Does not throw, returns socket_address.
socket_address::any(socket_protocol::ipv4);
socket_address::any(socket_protocol::ipv4, 8086);
socket_address::loopback(socket_protocol::ipv4);
socket_address::loopback(socket_protocol::ipv4, 8086);

// Does not throw, returns expected<socket_address, error>.
socket_address::from_string(socket_protocol::ipv4, "127.0.0.1", 8086);
```

### Socket Operations

There are four main operations for UDP sockets:

- binding
- address query
- sending a packet
- receiving a packet

The interface for each operation is very simple &mdash; the return value is either an `error` or an `expected`.

`wadjet::expected` is a simple implementation of `std::expected` which has been proposed to enter the C++ standard sometime in the future. Since no official implementation is currently available, `wadjet` provides one sufficient for internal needs.

>Current `wadjet::expected` implementation is very basic and should be used carefully. Hopefully, this will be improved in future versions.

### Binding a Socket

`socket::bind` takes a `socket_address` argument and returns an error code.

```C++
error bind_error = socket.bind(address);
```

### Address Query

`socket::address` query returns a `expected<socket_address, error>`, since it can fail under some circumstances.

```C++
expected<socket_address, error> query_result = socket.address();
if(query_result)
{
    socket_address& address = *query_result;

    // ... do something with address
}
else
{
    error& error = query_result.error();

    // ... do something with error
}
```

### Sending Data

`socket::send` returns a `wadjet::error` and takes two arguments:

- a `socket_address`, representing the destination
- a `std::span`, representing the data to be sent

The example of sending data is as follows:
```C++
std::string_view message = "HELLO";

error send_error = socket.send(address, std::span{message});
if(send_error != error_code::none)
{
    std::cerr << send_error.description() << std::endl;
    return 1;
}
```

### Receiving Data

`wadjet::recv` returns a `wadjet::expected` which contains a `wadjet::packet` if succeeds.

The simplified view of the packet structure is as follows:

```
struct packet
{
    socket_address  address;
    std::span<char> payload;
};
```

Notice the data is not stored in the `packet` structure itself &mdash; the user is expected to provide a buffer for storage, and `wadjet` merely provides a view into that buffer.

> `wadjet` sockets are non-blocking by design. If `recv` is called, but there are no UDP packets waiting to be processed, it will return `error_code::socket_would_block`.

The usage is as follows:

```C++
std::array<char, 1024> buffer;
auto result = socket.recv(std::span{buffer});
if(result)
{
    // Not very secure, but just an example :D
    std::cout << result->payload.data() << std::endl;
}
else
{
    // Absence of incoming packets isn't dangerous.
    if(result.error() != error_code::socket_would_block)
    {
        std::cerr << result.error().description() << std::endl;
    }
}
```

## Building

CMake configuration options:

- `WADJET_STATIC` - builds `wadjet` as a static instead of shared library
- `WADJET_BUILD_TESTS` - builds automated tests and enables ctest
- `WADJET_BUILD_EXAMPLES` - builds example applications

`wadjet` contains no external dependencies apart from STL and the underlying socket API libraries &mdash; this is all taken care of in CMake configurations.

Out-of-source builds are recommended, e.g.:

```bash
mkdir build
cd build
cmake -DWADJET_BUILD_TESTS=ON ..
make
```

## Improvements

`wadjet` is open to contributions. If you have any suggestions and think you can make it better, it's open season on pull requests.

Planned improvements include, but are not limited to:

- improve test coverage
- check potential thread-safety issues once need emerges
- provide more socket features (expose file descriptor flags and such, rather than setting them internally)
- better error encapsulation, do not leak implementation details (enumerate all relevant underlying errors into `wadjet::error`)
- improve `wadjet::expected` implementation while `std::expected` isn't available
- replace `wadjet::expected` with `std::expected` once C++23 becomes mainstream
- quality of life improvements &mdash; address-related utilities and similar

## FAQ

1. Why UDP only?

    Because I designed the library for my personal projects where I use UDP. TCP sockets should be considered out of scope for this project, at least for the time being.

2. Why are all `wadjet` sockets non-blocking?

    Same as above &mdash; it merely suits my needs. However, blocking can easily be added as a flag to `wadjet::socket` should need arise.

3. Can I use `wadjet` in my projects?

    Yes, read the `LICENSE`.

4. _Should_ I use `wadjet` in my projects?

    At the moment, you can expect the interfaces to be volatile and prone to change, zero support, erratic PR review activity, similar &mdash; so I don't advise using it in a serious application.

    At least not yet.

4. How do I contribute?

    Simply create a pull request with your changes, after which they will be reviewed and merged.

    There is no coding standard at the time, but try adapting the style you see throughout the codebase. Also, there is a `.clang-format`, which should be used.
