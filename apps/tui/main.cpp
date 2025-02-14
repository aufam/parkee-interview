#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <parkee-interview/sync_deque.h>
#include <condition_variable>

using namespace Project;
using parkee::SyncDeque;
namespace tui = ftxui;

auto Tab(const std::vector<std::string>& titles, int& index, tui::Components contents) -> tui::Component;
auto RawData(SyncDeque&) -> tui::Component;

auto DataCollection(
        tui::ScreenInteractive& screen,
        SyncDeque& deque,
        std::string& port,
        std::string& port_placeholder,
        std::string& info
) -> tui::Component;

auto Backend(
        tui::ScreenInteractive& screen,
        SyncDeque& deque,
        std::condition_variable& cv,
        std::atomic_bool& is_running,
        std::atomic_bool& is_connected,
        std::string& port,
        std::string& info
) -> std::jthread;

bool EventHandler(const tui::Event& e, SyncDeque& deque, std::condition_variable& cv, std::atomic_bool& is_connected);

int main() {
    auto screen = tui::ScreenInteractive::Fullscreen();

    auto deque = SyncDeque(
        40,  // capacity
        [&] { screen.PostEvent(tui::Event::Special("on_push")); }, // on_push
        [&] { screen.PostEvent(tui::Event::Special("on_clear")); } // on_clear
    );

    // some global variables
    std::string input_port;
    std::string input_port_placeholder = "Enter serial port here";
    std::string info = "Disconnected";

    std::condition_variable cv;
    std::atomic_bool is_running{true};
    std::atomic_bool is_connected{false};

    auto backend = Backend(screen, deque, cv, is_running, is_connected, input_port, info);

    // create tab component
    auto tab_index = 0;
    const auto tab_titles = std::vector<std::string>({
        "Data Collection",
        "Raw Data",
    });
    auto tab = Tab(tab_titles, tab_index, {
        DataCollection(screen, deque, input_port, input_port_placeholder, info),
        RawData(deque),
    });

    // create window component
    auto window = tui::Renderer(tab, [&] {
        return tui::window(
            tui::text(" " PARKEE_PROJECT_NAME " -- v" PARKEE_PROJECT_VERSION " ") | tui::bold | tui::center,
            tab->Render()
        );
    });

    window |= tui::CatchEvent([&](tui::Event e) {
        return EventHandler(e, deque, cv, is_connected);
    });

    screen.Loop(window);

    std::lock_guard<std::mutex> lock(deque.get_mutex());
    is_running.store(false);
    is_connected.store(false);
    cv.notify_one();

    return 0;
}

