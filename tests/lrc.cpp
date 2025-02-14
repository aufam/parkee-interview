#include <catch2/catch_test_macros.hpp>
#include <parkee-interview/lrc.h>

using namespace Project;

TEST_CASE("Parkee", "[lrc]") {
    REQUIRE(parkee::lrc::encode({}) == 0x00);
    REQUIRE(parkee::lrc::encode({0xaa}) == 0xaa);
    REQUIRE(parkee::lrc::encode({0xaa, 0xbb}) == (0xaa ^ 0xbb));
}

