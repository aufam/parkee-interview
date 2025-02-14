#include <fmt/chrono.h>
#include <parkee-interview/payload.h>
#include <parkee-interview/lrc.h>
#include <random>

using namespace Project;
using delameta::Result, delameta::Error;
using etl::Ok, etl::Err;
using unix_time_unit = std::chrono::seconds;

size_t parkee::Payload::cnt = 0;
float parkee::Payload::average = 0;
float parkee::Payload::max = NAN;
float parkee::Payload::min = NAN;

parkee::Payload::Payload(std::chrono::system_clock::time_point timestamp, float value)
    : timestamp(timestamp)
    , value(value)
{
    // moving average
    cnt++;
    average = ((average * (cnt - 1)) + value) / cnt;

    // max and min
    max = std::isnan(max) ? value : max > value ? max : value;
    min = std::isnan(min) ? value : min < value ? min : value;
}

float parkee::Payload::get_average() { return average; }
float parkee::Payload::get_max() { return max; }
float parkee::Payload::get_min() { return min; }
void parkee::Payload::reset() { min = max = NAN; average = cnt = 0; }

auto parkee::Payload::New(float value) -> Payload {
    return {std::chrono::system_clock::now(), value};
}

auto parkee::Payload::Random() -> Payload {
    auto rng = std::mt19937(static_cast<unsigned>(std::time(nullptr)));
    auto dist = std::uniform_int_distribution<int>(-1000, 1000);
    return {std::chrono::system_clock::now(), float(dist(rng))/1000.f};
}

auto parkee::Payload::serialize() const -> std::vector<uint8_t> {
    std::vector<uint8_t> data(Payload::Size);
    data[0] = 0x02;
    int idx = 1;

    uint64_t tp = std::chrono::duration_cast<unix_time_unit>(timestamp.time_since_epoch()).count();
    for (int i = 0; i < 8; ++i)
        data[idx++] = static_cast<uint8_t>((tp >> (56 - i * 8)) & 0xFF);

    uint32_t f_bits;
    std::memcpy(&f_bits, &value, sizeof(f_bits));
    for (int i = 0; i < 4; ++i)
        data[idx++] = static_cast<uint8_t>((f_bits >> (24 - i * 8)) & 0xFF);

    data[idx++] = lrc::encode(data.begin() + Payload::PayloadOffset, data.begin() + Payload::LRCOffset + 1);
    data[idx] = 0x03;
    return data;
}

auto parkee::Payload::deserialize(const std::vector<uint8_t>& data) -> Result<Payload> {
    if (data.size() != Payload::Size)
        return Err(Error{-1,
            fmt::format("Invalid frame size, expect {} got {}", Payload::Size, data.size())
        });

    if (data[0] != Payload::StartByte)
        return Err(Error{-1,
            fmt::format("Invalid start byte, expect 0x{:02X} got 0x{:02X}", Payload::StartByte, data[0])
        });

    if (data[Payload::Size - 1] != Payload::StopByte)
        return Err(Error{-1,
            fmt::format("Invalid stop byte, expect 0x{:02X} got 0x{:02X}", Payload::StopByte, data[Payload::Size - 1])
        });

    const auto expected_lrc = lrc::encode(data.begin() + Payload::PayloadOffset, data.begin() + Payload::LRCOffset);
    const auto actual_lrc = data[Payload::LRCOffset];
    if (expected_lrc != actual_lrc)
        return Err(Error{-1,
            fmt::format("Invalid LRC, expect 0x{:02X} got 0x{:02X}", expected_lrc, actual_lrc)
        });

    int idx = 1;
    uint64_t tp = 0;
    float value;

    for (int i = 0; i < 8; ++i)
        tp |= static_cast<uint64_t>(data[idx++]) << (56 - i * 8);

    uint32_t f_bits = 0;
    for (int i = 0; i < 4; ++i)
        f_bits |= static_cast<uint32_t>(data[idx++]) << (24 - i * 8);
    std::memcpy(&value, &f_bits, sizeof(f_bits));

    return Ok(Payload{std::chrono::system_clock::time_point{unix_time_unit{tp}}, value});
}
