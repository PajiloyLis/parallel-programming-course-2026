#include "ShardedCollector.hpp"

#include <algorithm>
#include "ShardedCollector.hpp"
#include "PercentileCalc.hpp"

void ShardedCollector::record(uint64_t value) {
    uint64_t b = std::min(value / 4, (uint64_t) 255);

    {
        std::lock_guard<std::mutex> g(stripes_[b % NUM_STRIPES]);
        buckets_[b]++;
    }

    count_.fetch_add(1, std::memory_order_relaxed);
    sum_.fetch_add(value, std::memory_order_relaxed);

    uint64_t cur = min_.load(std::memory_order_relaxed);
    while (value < cur) {
        if (min_.compare_exchange_weak(
            cur, value,
            std::memory_order_relaxed,
            std::memory_order_relaxed))
            break;
    }

    cur = max_.load(std::memory_order_relaxed);
    while (value > cur) {
        if (max_.compare_exchange_weak(
            cur, value,
            std::memory_order_relaxed,
            std::memory_order_relaxed))
            break;
    }
}

Snapshot ShardedCollector::snapshot() {
    Snapshot s;
    s.buckets.fill(0);

    for (size_t g = 0; g < NUM_STRIPES; ++g) {
        std::lock_guard<std::mutex> lock(stripes_[g]);
        for (size_t i = g; i < BUCKETS; i += NUM_STRIPES)
            s.buckets[i] = buckets_[i];
    }

    s.count = count_.load(std::memory_order_relaxed);
    s.sum = sum_.load(std::memory_order_relaxed);
    s.min = min_.load(std::memory_order_relaxed);
    s.max = max_.load(std::memory_order_relaxed);
    if (s.count == 0) s.min = 0;

    s.p50 = computePercentile(s.buckets, s.count, 0.50);
    s.p99 = computePercentile(s.buckets, s.count, 0.99);
    return s;
}
