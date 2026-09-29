#ifndef ITMO_PARALLELPROG_SIMPLECOLLECTOR_HPP
#define ITMO_PARALLELPROG_SIMPLECOLLECTOR_HPP

#include "MetricsCollector.hpp"

class SimpleCollector : public MetricsCollector {
public:
    void record(uint64_t value) override;

    Snapshot snapshot() override ;

private:
    std::array<uint64_t, 256> buckets_{};
    uint64_t count_ = 0, sum_ = 0;
    uint64_t min_ = UINT64_MAX, max_ = 0;
};

#endif //ITMO_PARALLELPROG_SIMPLECOLLECTOR_HPP