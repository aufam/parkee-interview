#include <boost/preprocessor.hpp>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <delameta/serial.h>
#include <delameta/opts.h>
#include <delameta/debug.h>
#include <parkee-interview/payload.h>
#include <parkee-interview/sync_deque.h>
#include <atomic>
#include <csignal>

using namespace Project;
using delameta::Result, delameta::Serial;
using etl::Ok, etl::Err;
using parkee::Payload, parkee::SyncDeque;

static std::atomic_bool is_running{true};

OPTS_MAIN(
    (ParkeeReceiver, "Parkee Serial Receiver")
    ,
    /*   Type   |   Arg   | Short |  Long     |              Help                | Default */
    (std::string, port    ,  'p'  , "port"    , "Specify serial port"                       )
    (int        , baud    ,  'b'  , "baud"    , "Specify serial baud rate"       , "9600"   )
    (size_t     , capacity,  'n'  , "capacity", "Recent values buffer capacity"  , "15"     )
    ,
    (Result<void>)
) {
    auto serial = TRY(Serial::Open(Serial::Args{.port=port, .baud=baud, .timeout=1}));
    fmt::println("Listening on port {}", port);

    SyncDeque recent(capacity, []{}, []{});
    std::signal(SIGINT, [](int) { is_running = false; });

    while (is_running) {
        auto raw = TRY_OR(serial.read(), continue); // read or continue
        fmt::print("\033[2J\033[H"); // clear screen

        auto payload = Payload::deserialize(raw);
        recent.push(std::move(raw), std::move(payload));

        fmt::println("Avg: {}, Min: {}, Max: {}", Payload::get_average(), Payload::get_min(), Payload::get_max());
        recent.foreach([](const auto& raw, const auto& payload) {
            fmt::println("Raw: {:02X}, {}", fmt::join(raw, " "), payload);
        });
    }

    return Ok();
}
