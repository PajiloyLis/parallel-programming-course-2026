#ifndef ITMO_PARALLELPROG_SHARDEDCOLLECTOR_H
#define ITMO_PARALLELPROG_SHARDEDCOLLECTOR_H

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>

#include "MetricsCollector.hpp"

class ShardedCollector : public MetricsCollector {
public:
    void record(uint64_t value) override;
    Snapshot snapshot() override;

private:
    static constexpr size_t NUM_STRIPES = 16;
    static constexpr size_t BUCKETS = 256;

    std::array<uint64_t, BUCKETS> buckets_{};
    std::array<std::mutex, NUM_STRIPES> stripes_;

    std::atomic<uint64_t> count_{0};
    std::atomic<uint64_t> sum_{0};
    std::atomic<uint64_t> min_{UINT64_MAX};
    std::atomic<uint64_t> max_{0};
};

#endif //ITMO_PARALLELPROG_SHARDEDCOLLECTOR_H