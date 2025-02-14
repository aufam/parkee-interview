#include <ftxui/component/component.hpp>
#include <parkee-interview/payload.h>
#include <parkee-interview/sync_deque.h>

using namespace Project;
using parkee::Payload, parkee::SyncDeque;
namespace tui = ftxui;

static void draw_point(tui::Canvas& canvas, const Payload& payload, int& idx) {
    int w = canvas.width();
    int h = canvas.height() / 2;

    int y2 = -payload.value * h + h;
    canvas.DrawBlock(w - idx - 1, y2, true, tui::Color::Red);
    idx += 5;
}

// canvas renderer
auto Canvas(SyncDeque& deque) -> tui::Component {
    return tui::Renderer([&] {
        auto canvas = tui::Canvas(200, 50);

        // draw y = 0
        canvas.DrawPointLine(0, canvas.height() / 2, canvas.width(), canvas.height() / 2);

        int idx = 0;
        deque.foreach([&](const auto&, const auto& payload) {
            if (payload.is_ok()) draw_point(canvas, payload.unwrap(), idx);
        });

        return tui::hbox({
            tui::vbox({
                tui::text("1"),
                tui::filler(),
                tui::text("0"),
                tui::filler(),
                tui::text("-1"),
            }),
            tui::window(tui::text("Live chart") | tui::center, tui::canvas(std::move(canvas))),
        });
    });
}
