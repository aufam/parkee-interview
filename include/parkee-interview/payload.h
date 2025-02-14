#ifndef PARKEE_INTERVIEW_PAYLOAD_H
#define PARKEE_INTERVIEW_PAYLOAD_H

#include <delameta/error.h>
#include <chrono>
#include <vector>

namespace Project::parkee {
    /// payload data structure
    struct Payload {
        // some constants
        static constexpr uint8_t StartByte = 0x02;
        static constexpr uint8_t StopByte = 0x03;
        static constexpr size_t PayloadOffset = 1;
        static constexpr size_t LRCOffset = PayloadOffset + 8 + 4;
        static constexpr size_t Size = 1 + 8 + 4 + 1 + 1;

        // payload fields
        std::chrono::system_clock::time_point timestamp;
        float value;

        // payload factories
        static Payload New(float value);
        static Payload Random();

        // serde
        static delameta::Result<Payload> deserialize(const std::vector<uint8_t>&);
        std::vector<uint8_t> serialize() const;

        // statistics
        static float get_average();
        static float get_max();
        static float get_min();
        static void reset();

    private:
        Payload(std::chrono::system_clock::time_point timestamp, float value);

        static size_t cnt;
        static float average;
        static float max;
        static float min;
    };
}

// formatter trait
#ifdef FMT_CHRONO_H_
template <> struct fmt::formatter<Project::parkee::Payload> {
    using Self = Project::parkee::Payload;
    constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

    template <typename Ctx>
    inline auto format(const Self& self, Ctx& ctx) const {
        auto it = ctx.out();
        return fmt::format_to(it,
            "Payload {{timestamp: {}, value: {:.3f}}}",
            std::chrono::time_point_cast<std::chrono::seconds>(self.timestamp), self.value
        );
    }
};
#endif

#endif

