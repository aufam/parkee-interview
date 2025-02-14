#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <delameta/debug.h> // enable advance formatter

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <parkee-interview/payload.h>
#include <parkee-interview/sync_deque.h>
#include <delameta/serial.h>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace Project;
using parkee::Payload, parkee::SyncDeque;
using delameta::Serial;
namespace tui = ftxui;

std::condition_variable cv;

void Popup(std::string title, std::string message);

static void backend_task(
        tui::ScreenInteractive& screen,
        SyncDeque& deque,
        std::condition_variable& cv,
        std::atomic_bool& is_running,
        std::atomic_bool& is_connected,
        std::string& port,
        std::string& info
) {
    while (is_running) {
        // wait for connection signal
        {
            std::unique_lock<std::mutex> lock(deque.get_mutex());
            cv.wait(lock, [&] { return is_connected.load() || !is_running.load(); });

            if (!is_running.load()) break;
        }

        // try to open serial port
        auto [serial, err] = Serial::Open(Serial::Args{.port=port, .baud=9600, .timeout=1});
        if (err) {
            screen.Post([err=std::move(*err)] {
                Popup("Fatal Error", "Failed to open serial port: " + err.what);
            });
            screen.Post(tui::Event::Custom);

            is_connected.store(false);
            continue;
        }

        // update info message
        screen.Post([&, port=port] { info = "Connected to " + port; });
        screen.Post(tui::Event::Custom);

        // read data while connected
        while (is_connected.load()) {
            auto raw = TRY_OR(serial->read(), continue); // read or continue

            auto payload = Payload::deserialize(raw);
            auto str = payload.is_ok() ?
                fmt::format("Added new item: {}", payload.unwrap()) :
                fmt::format("Fail to deserialize payload: {}", payload.unwrap_err());

            deque.push(std::move(raw), std::move(payload));

            screen.Post([&, str=str] { info = str; });
            screen.Post(tui::Event::Custom);
        }

        screen.Post([&] { info = "Disconnected"; });
        screen.Post(tui::Event::Custom);
    }
}

auto Backend(
        tui::ScreenInteractive& screen,
        SyncDeque& deque,
        std::condition_variable& cv,
        std::atomic_bool& is_running,
        std::atomic_bool& is_connected,
        std::string& port,
        std::string& info
) -> std::jthread {
    return std::jthread([&] {
        backend_task(screen, deque, cv, is_running, is_connected, port, info);
    });
}

