#include <parkee-interview/sync_deque.h>

using namespace Project;
using delameta::Result;

parkee::SyncDeque::SyncDeque(size_t capacity, std::function<void()>&& on_push, std::function<void()>&& on_clear)
    : on_push(std::move(on_push))
    , on_clear(std::move(on_clear))
    , capacity(capacity)
{}

void parkee::SyncDeque::push(std::vector<uint8_t>&& raw, Result<Payload>&& res) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.emplace_back(std::move(raw), std::move(res));
        if (buffer.size() > capacity)
            buffer.pop_front();
    }

    on_push();
}

void parkee::SyncDeque::clear() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.clear();
    }

    on_clear();
}

void parkee::SyncDeque::foreach(const std::function<void(const std::vector<uint8_t>&, const Result<Payload>&)>& f, bool reverse) const {
    std::lock_guard<std::mutex> lock(mtx);
    if (not reverse)
        for (const auto &[raw, data]: buffer) f(raw, data);
    else
        for (auto begin = buffer.rbegin(); begin != buffer.rend(); ++begin) f(begin->first, begin->second);
}

size_t parkee::SyncDeque::size() const {
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.size();
}

std::mutex& parkee::SyncDeque::get_mutex() const {
    return mtx;
}
