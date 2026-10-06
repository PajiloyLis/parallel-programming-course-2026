#include "ThreadLocalCollector.h"

#include "../Step2/PercentileCalc.hpp"

inline void relaxed_add(std::atomic<uint64_t> &c, uint64_t delta) {
    c.store(c.load(std::memory_order_relaxed) + delta, std::memory_order_relaxed);
}

void ThreadLocalCollector::record(uint64_t value) {
    ThreadState *s = get_my_state();
    uint64_t b = std::min(value / 4, (uint64_t) 255);

    relaxed_add(s->buckets[b], 1);
    relaxed_add(s->count, 1);
    relaxed_add(s->sum, value);

    if (value < s->min.load(std::memory_order_relaxed))
        s->min.store(value, std::memory_order_relaxed);
    if (value > s->max.load(std::memory_order_relaxed))
        s->max.store(value, std::memory_order_relaxed);
}

Snapshot ThreadLocalCollector::snapshot() {
    std::vector<ThreadState *> all_states_copy;
    {
        std::lock_guard<std::mutex> g(list_lock);
        all_states_copy.reserve(all_states.size());
        for (auto &state: all_states) {
            all_states_copy.push_back(state.get());
        }
    }

    Snapshot s;
    s.buckets.fill(0);
    uint64_t count = 0, sum = 0, mn = UINT64_MAX, mx = 0;

    for (auto *state: all_states_copy) {
        for (int i = 0; i < 256; i++) {
            s.buckets[i] += state->buckets[i].load(std::memory_order_relaxed);
        }
        sum += state->sum.load(std::memory_order_relaxed);
        count += state->count.load(std::memory_order_relaxed);
        mn = std::min(mn, state->min.load(std::memory_order_relaxed));
        mx = std::max(mx, state->max.load(std::memory_order_relaxed));
    }
    s.sum = sum, s.count = count, s.min = mn, s.max = mx;
    s.p50 = computePercentile(s.buckets, s.count, 0.50);
    s.p99 = computePercentile(s.buckets, s.count, 0.99);
    return s;
}

ThreadState *ThreadLocalCollector::get_my_state() {
    struct TLSSlot {
        uint64_t id = 0;
        ThreadState *state = nullptr;
    };
    static thread_local TLSSlot slot;
    if (slot.id != id) {
        auto s = std::make_unique<ThreadState>();
        ThreadState *raw = s.get();
        {
            std::lock_guard<std::mutex> g(list_lock);
            all_states.push_back(std::move(s));
        }
        slot.id = id;
        slot.state = raw;
    }
    return slot.state;
}
