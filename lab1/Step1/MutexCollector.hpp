#ifndef ITMO_PARALLELPROG_MUTEXCOLLECTOR_HPP
#define ITMO_PARALLELPROG_MUTEXCOLLECTOR_HPP

#include <mutex>

#include "MetricsCollector.hpp"

class MutexCollector : public MetricsCollector {
public:
    void record(uint64_t value) override;

    Snapshot snapshot() override ;

private:
    mutable std::mutex mtx;
    std::array<uint64_t, 256> buckets_{};
    uint64_t count_ = 0, sum_ = 0;
    uint64_t min_ = UINT64_MAX, max_ = 0;
};

#endif //ITMO_PARALLELPROG_MUTEXCOLLECTOR_HPP