#include <boost/preprocessor.hpp>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <delameta/serial.h>
#include <delameta/opts.h>
#include <delameta/debug.h>
#include <parkee-interview/payload.h>

using namespace Project;
using delameta::Result, delameta::Serial;
using parkee::Payload;

OPTS_MAIN(
    (ParkeeSender, "ParkeeSerialSender")
    ,
    /*   Type   |  Arg  | Short |  Long   |              Help               |      Default   */
    (bool       , random,  'r'  , "random", "Generate randomize payload"                       )
    (float      , value ,  'v'  , "value" , "Specify payload value"         , "0"              )
    (bool       , broken,  'k'  , "broken", "Invalidate the LRC value to simulate broken frame")
    (std::string, port  ,  'p'  , "port"  , "Specify serial port"                              )
    (int        , baud  ,  'b'  , "baud"  , "Specify serial baud rate"      , "9600"           )
    ,
    (Result<void>)
) {
    auto payload = random ? Payload::Random() : Payload::New(value);
    fmt::println("Sending {} through {}...", payload, port);

    auto res = Serial::Open(Serial::Args{.port=port, .baud=baud, .timeout=1})
        .and_then([&](Serial serial) {
            auto data = payload.serialize();
            if (broken) data[Payload::LRCOffset] = ~data[Payload::LRCOffset];
            fmt::println("Raw: {:02X}", fmt::join(data, " "));
            return serial.write(data);
        });

    fmt::println("{}", res);

    return res;
}
