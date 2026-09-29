#ifndef ITMO_PARALLELPROG_EMPTYMUTEXCOLLECTOR_H
#define ITMO_PARALLELPROG_EMPTYMUTEXCOLLECTOR_H

#include <mutex>
#include "MetricsCollector.hpp"

class EmptyLockCollector : public MetricsCollector {
public:
    void record(uint64_t) override {
        std::lock_guard<std::mutex> g(mtx_);
    }

    Snapshot snapshot() override {
        return {};
    }

private:
    std::mutex mtx_;
};

#endif //ITMO_PARALLELPROG_EMPTYMUTEXCOLLECTOR_H
