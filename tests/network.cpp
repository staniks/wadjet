#include "catch_amalgamated.hpp"

#include <wadjet/socket.hpp>
#include <wadjet/network.hpp>

using namespace wadjet;

TEST_CASE("socket address from string", "[socket_address]")
{
    // Scope IPV4.
    {
        auto address = socket_address::from_string(socket_protocol::ipv4, "127.0.0.1");
        REQUIRE(address);

        auto invalid_address = socket_address::from_string(socket_protocol::ipv4, "should fail");
        REQUIRE(!invalid_address);
    }

    // Scope IPV6.
    {
        auto address = socket_address::from_string(socket_protocol::ipv6, "::ffff:127.0.0.1");
        REQUIRE(address);

        auto invalid_address = socket_address::from_string(socket_protocol::ipv6, "should fail");
        REQUIRE(!invalid_address);
    }
}

TEST_CASE("socket address from string (constructed)", "[socket_address]")
{
    // Scope IPV4.
    {
        CHECK_NOTHROW(socket_address{socket_protocol::ipv4, "127.0.0.1"});
        CHECK_THROWS(socket_address{socket_protocol::ipv4, "this should fail"});
    }

    // Scope IPV6.
    {
        CHECK_NOTHROW(socket_address{socket_protocol::ipv6, "::ffff:127.0.0.1"});
        CHECK_THROWS(socket_address{socket_protocol::ipv6, "this should fail"});
    }
}

TEST_CASE("socket address to string", "[socket_address]")
{
    std::array<char, 1024> buffer;

    // Scope IPV4.
    {
        auto address = socket_address::from_string(socket_protocol::ipv4, "127.0.0.1");
        REQUIRE(address);

        auto result = address->to_string(std::span(buffer));
        REQUIRE(result);
        CHECK(*result == "127.0.0.1");
    }

    // Scope IPV6.
    {
        auto address = socket_address::from_string(socket_protocol::ipv6, "::ffff:127.0.0.1");
        REQUIRE(address);

        auto result = address->to_string(std::span(buffer));
        REQUIRE(result);
        CHECK(*result == "::ffff:127.0.0.1");
    }
}

TEST_CASE("socket address copy", "[socket_address]")
{
    std::array<char, 1024> buffer;

    // Scope IPV4.
    {
        auto address = socket_address::from_string(socket_protocol::ipv4, "127.0.0.1");
        REQUIRE(address);

        socket_address new_address = *address;

        auto ip_string = new_address.to_string(std::span(buffer));
        REQUIRE(ip_string);
        CHECK(*ip_string == "127.0.0.1");
    }

    // Scope IPV6.
    {
        auto address = socket_address::from_string(socket_protocol::ipv6, "::ffff:127.0.0.1");
        REQUIRE(address);

        socket_address new_address = *address;

        auto ip_string = new_address.to_string(std::span(buffer));
        REQUIRE(ip_string);
        CHECK(*ip_string == "::ffff:127.0.0.1");
    }
}

TEST_CASE("socket address move", "[socket_address]")
{
    std::array<char, 1024> buffer;

    // Scope IPV4.
    {
        auto address = socket_address::from_string(socket_protocol::ipv4, "127.0.0.1");
        REQUIRE(address);

        socket_address new_address = std::move(*address);

        auto ip_string = new_address.to_string(std::span(buffer));
        REQUIRE(ip_string);
        CHECK(*ip_string == "127.0.0.1");
    }

    // Scope IPV6.
    {
        auto address = socket_address::from_string(socket_protocol::ipv6, "::ffff:127.0.0.1");
        REQUIRE(address);

        socket_address new_address = std::move(*address);

        auto ip_string = new_address.to_string(std::span(buffer));
        REQUIRE(ip_string);
        CHECK(*ip_string == "::ffff:127.0.0.1");
    }
}