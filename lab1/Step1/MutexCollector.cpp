#include <cstddef>

#include "MutexCollector.hpp"

uint64_t computePercentile(const std::array<uint64_t, 256> &buckets,
                           uint64_t count, double p);

void MutexCollector::record(uint64_t value) {
    std::lock_guard guard(mtx);
    uint64_t b = std::min(value / 4, (uint64_t) 255);
    buckets_[b]++;
    count_++;
    sum_ += value;
    if (value < min_) min_ = value;
    if (value > max_) max_ = value;
}

Snapshot MutexCollector::snapshot() {
    std::lock_guard guard(mtx);
    Snapshot s;
    s.buckets = buckets_;
    s.count = count_;
    s.sum = sum_;
    s.min = min_;
    s.max = max_;
    s.p50 = computePercentile(buckets_, count_, 0.50);
    s.p99 = computePercentile(buckets_, count_, 0.99);
    return s;
}

uint64_t computePercentile(const std::array<uint64_t, 256> &buckets,
                           uint64_t count, double p) {
    if (count == 0)
        return 0;
    double threshold = count * p;
    size_t accumulated = 0;
    for (size_t i = 0; i < 256; ++i) {
        accumulated += buckets[i];
        if (accumulated >= threshold)
            return i * 4;
    }
    return 255 * 4;
}
