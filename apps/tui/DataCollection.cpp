#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <parkee-interview/payload.h>
#include <parkee-interview/sync_deque.h>

using namespace Project;
using parkee::Payload, parkee::SyncDeque;
using etl::Ok;
namespace tui = ftxui;

auto Canvas(SyncDeque& deque) -> tui::Component;

/// data collection renderer
auto DataCollection(
        tui::ScreenInteractive& screen,
        SyncDeque& deque,
        std::string& port,
        std::string& port_placeholder,
        std::string& info
) -> tui::Component {
    auto randomize = [&]{
        auto payload = Payload::Random();
        deque.push(payload.serialize(), Ok(payload));
    };

    tui::InputOption input_option;
    input_option.on_enter = [&]{
        port.pop_back(); // remove '\n'
        screen.Post(tui::Event::Special("Start"));
    };

    return tui::Container::Vertical({
        tui::Input(&port, &port_placeholder, input_option),
        tui::Renderer(tui::separatorEmpty),
        tui::Container::Horizontal({
            tui::Button("Start", [&]{ screen.Post(tui::Event::Special("Start")); }),
            tui::Button("Stop", [&]{ screen.Post(tui::Event::Special("Stop")); }),
            tui::Button("Clear", [&]{ screen.Post(tui::Event::Special("Clear")); }),
            tui::Button("Randomize", randomize),
        }),
        Canvas(deque),
        tui::Renderer(tui::separatorEmpty),
        tui::Renderer([&]{
            return tui::text(info.empty() ? "" : "Info: " + info) | tui::color(tui::Color::Yellow);
        }),
    });
}
