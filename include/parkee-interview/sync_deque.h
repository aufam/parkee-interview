#ifndef PARKEE_INTERVIEW_SYNC_DEQUE_H
#define PARKEE_INTERVIEW_SYNC_DEQUE_H

#include "parkee-interview/payload.h"
#include <deque>
#include <functional>
#include <mutex>

namespace Project::parkee {
    /// thread-safe double ended queue for storing raw data and its deserialize result
    class SyncDeque {
        std::deque<std::pair<std::vector<uint8_t>, delameta::Result<Payload>>> buffer;
        mutable std::mutex mtx;

        // push and clear callback as class members instead of virtual functions
        std::function<void()> on_push;
        std::function<void()> on_clear;

    public:
        SyncDeque(size_t capacity, std::function<void()>&& on_push, std::function<void()>&& on_clear);

        /// safely push a new item and call on_push()
        void push(std::vector<uint8_t>&& raw, delameta::Result<Payload>&& data);

        /// safely clear buffer and call on_push()
        void clear();

        /// safely iterate over raw and payload data and execute a function
        void foreach(const std::function<void(const std::vector<uint8_t>&, const delameta::Result<Payload>&)>&) const;

        size_t size() const; //< get buffer size
        size_t capacity;     //< get and set capacity
    };
}

#endif

