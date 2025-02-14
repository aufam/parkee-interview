#include <catch2/catch_test_macros.hpp>
#include <fmt/chrono.h>
#include <parkee-interview/payload.h>
#include <parkee-interview/lrc.h>
#include <cmath>

using namespace Project;

TEST_CASE("Parkee", "[payload]") {
    REQUIRE(parkee::Payload::get_average() == 0);
    REQUIRE(std::isnan(parkee::Payload::get_min()));
    REQUIRE(std::isnan(parkee::Payload::get_max()));

    auto a = parkee::Payload::Random();
    REQUIRE(parkee::Payload::get_average() == a.value);
    REQUIRE(parkee::Payload::get_min() == a.value);
    REQUIRE(parkee::Payload::get_max() == a.value);
}

TEST_CASE("Parkee", "[serde]") {
    auto a = parkee::Payload::Random();
    auto data = a.serialize();

    auto expected_lrc = parkee::lrc::encode(
        data.begin() + parkee::Payload::PayloadOffset,
        data.begin() + parkee::Payload::LRCOffset
    );
    auto &actual_lrc = data[parkee::Payload::LRCOffset];
    REQUIRE(expected_lrc == actual_lrc);

    auto b = parkee::Payload::deserialize(data);
    REQUIRE(b.is_ok());
    REQUIRE(fmt::to_string(b.unwrap()) == fmt::to_string(a));

    actual_lrc = ~actual_lrc; // broken lrc
    auto c = parkee::Payload::deserialize(data);
    REQUIRE(c.is_err());
    REQUIRE(c.unwrap_err().what == fmt::format("Invalid LRC, expect 0x{:02X} got 0x{:02X}", expected_lrc, actual_lrc));
}

