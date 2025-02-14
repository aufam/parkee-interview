#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <delameta/debug.h> // enabling advance formatter

#include <ftxui/dom/elements.hpp>
#include <parkee-interview/payload.h>
#include <parkee-interview/sync_deque.h>
#include <ftxui/component/component.hpp>

using namespace Project;
using parkee::Payload, parkee::SyncDeque;
namespace tui = ftxui;

/// raw data text renderer
auto RawData(SyncDeque& deque) -> tui::Component {
    return tui::Renderer([&] {
        auto first_line = fmt::format(
            "Avg: {:.3f}, Min: {:0.3f}, Max: {:0.3f}",
            Payload::get_average(), Payload::get_min(), Payload::get_max()
        );
        auto elems = tui::Elements({
            tui::text(std::move(first_line)),
        });

        bool reverse = true;
        deque.foreach([&](const auto& raw, const auto& payload) {
            auto line = fmt::format(
                "Raw: {:02X}, {}",
                fmt::join(raw, " "), payload
            );
            auto color = tui::color(payload.is_err() ? tui::Color::Red : tui::Color::Green);
            elems.push_back(tui::text(std::move(line)) | color);
        }, reverse);

        return tui::vbox(std::move(elems)) | tui::vscroll_indicator | tui::frame;
    });
}

