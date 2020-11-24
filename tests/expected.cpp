#include "catch_amalgamated.hpp"

#include <wadjet/expected.hpp>

using namespace wadjet;

TEST_CASE("expected", "[expected]")
{
    expected<int, char> valid = 3;
    REQUIRE(valid);
    REQUIRE(valid.has_value());

    CHECK(*valid == 3);
    CHECK(valid.value() == 3);

    expected<int, char> invalid = make_unexpected<char>(69);
    REQUIRE(!invalid);
    REQUIRE(!invalid.has_value());

    CHECK(invalid.error() == 69);

    struct complex
    {
        char member = 3;
    };

    expected<complex, int> valid_complex = complex{};
    REQUIRE(valid_complex);
    CHECK((*valid_complex).member == 3);
    CHECK(valid_complex.value().member == 3);
    CHECK(valid_complex->member == 3);
}