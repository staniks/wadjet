#include "catch_amalgamated.hpp"

#include <wadjet/errors.hpp>

using namespace wadjet;

TEST_CASE("error tests", "[error]")
{
    error err{error_code::socket_bind_error, 123};
    CHECK(err.code == error_code::socket_bind_error);
    CHECK(err.underlying_code == 123);
    CHECK(*err == error_code::socket_bind_error);
    CHECK(err == error_code::socket_bind_error);
    CHECK(err != error_code::socket_creation_fail);
}
