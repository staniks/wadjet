#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch_amalgamated.hpp"

#include <wadjet/socket.hpp>
#include <array>
#include <cstring>

using namespace wadjet;

TEST_CASE("socket context container", "[socket_context]")
{
    CHECK_NOTHROW(socket_api{});
}

void test_socket_container(socket_protocol protocol, socket_flags flags)
{
    wadjet::socket_api socket_api;

    CHECK_NOTHROW(wadjet::socket{protocol, flags});
}

TEST_CASE("socket container ipv4", "[socket]")
{
    test_socket_container(socket_protocol::ipv4, socket_flags::none);
}

TEST_CASE("socket container ipv6", "[socket]")
{
    test_socket_container(socket_protocol::ipv6, socket_flags::none);
}

TEST_CASE("socket container ipv6 dual-stack", "[socket]")
{
    test_socket_container(socket_protocol::ipv6, socket_flags::dual_stack);
}

void test_successful_send_receive(socket_protocol sender_protocol,
                                  socket_flags    sender_flags,
                                  socket_protocol receiver_protocol,
                                  socket_flags    receiver_flags)
{
    wadjet::socket_api socket_api;

    socket sender   = socket{sender_protocol, sender_flags};
    socket receiver = socket{receiver_protocol, receiver_flags};

    // Attempt to listen.
    REQUIRE(receiver.bind(socket_address::any(receiver_protocol)) == error_code::none);
    auto receiver_address = receiver.address();
    REQUIRE(receiver_address);

    // Address protocol is the protocol of the destination, except in case when we're a IPV4 socket
    // and the destination is IPV6 - an IPV4 socket can't handle IPV6 address.
    auto address_protocol = receiver_protocol;
    if(sender_protocol == socket_protocol::ipv4 && address_protocol == socket_protocol::ipv6)
        address_protocol = socket_protocol::ipv4;

    // Since receiver socket is bound on the same machine, use loopback.
    auto address = socket_address::loopback(address_protocol, receiver_address->port_host_order());

    // Send a simple message.
    constexpr std::string_view message = "hello there";
    REQUIRE(sender.send(address, std::span(message)) == error_code::none);

    // Allocate a buffer for the receiver.
    std::array<char, message.size()> recv_buffer;

    // Check if there are any packets waiting and attempt to process one.
    auto result = receiver.recv(std::span(recv_buffer));
    REQUIRE(result);

    // Sizes of sent and received data should match.
    REQUIRE(result.value().payload.size() == message.size());

    // Contents of sent and received data should match.
    CHECK(std::strncmp(message.data(), recv_buffer.data(), message.size()) == 0);
}

TEST_CASE("socket IPV4 -> IPV4 send and receive", "[socket]")
{
    test_successful_send_receive(socket_protocol::ipv4,
                                 socket_flags::none,
                                 socket_protocol::ipv4,
                                 socket_flags::none);
}

TEST_CASE("socket IPV4 -> IPV6 send and receive", "[socket]")
{
    test_successful_send_receive(socket_protocol::ipv4,
                                 socket_flags::none,
                                 socket_protocol::ipv6,
                                 socket_flags::dual_stack | socket_flags::none);
}

TEST_CASE("socket IPV6 -> IPV4 send and receive", "[socket]")
{
    test_successful_send_receive(socket_protocol::ipv6,
                                 socket_flags::dual_stack | socket_flags::none,
                                 socket_protocol::ipv4,
                                 socket_flags::none);
}

TEST_CASE("socket IPV6 -> IPV6 send and receive", "[socket]")
{
    test_successful_send_receive(socket_protocol::ipv6,
                                 socket_flags::dual_stack | socket_flags::none,
                                 socket_protocol::ipv6,
                                 socket_flags::dual_stack | socket_flags::none);
}
