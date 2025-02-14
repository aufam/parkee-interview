#include <condition_variable>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <parkee-interview/sync_deque.h>

using namespace Project;
using parkee::SyncDeque;
namespace tui = ftxui;

bool EventHandler(const tui::Event& e, SyncDeque& deque, std::condition_variable& cv, std::atomic_bool& is_connected) {
    if (e.character() == "Start") {
        std::lock_guard<std::mutex> lock(deque.get_mutex());
        is_connected.store(true);
        cv.notify_one();
        return true;
    }

    if (e.character() == "Stop") {
        std::lock_guard<std::mutex> lock(deque.get_mutex());
        is_connected.store(false);
        cv.notify_one();
        return true;
    }

    if (e.character() == "Clear") {
        deque.clear();
        return true;
    }

    return false;
}

