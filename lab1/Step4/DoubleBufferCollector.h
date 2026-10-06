#ifndef ITMO_PARALLELPROG_DOUBLEBUFFERCOLLECTOR_H
#define ITMO_PARALLELPROG_DOUBLEBUFFERCOLLECTOR_H
#include <atomic>
#include <memory>
#include <vector>

#include "ThreadBuffers.h"
#include "../Step0/MetricsCollector.hpp"

inline uint64_t next_collector_id() {
    static std::atomic<uint64_t> counter{1};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

class DoubleBufferCollector : public MetricsCollector {
public:

    void record(uint64_t value) override;

    Snapshot snapshot() override;
private:
    ThreadBuffers *get_my_buffers();

    const uint64_t id = next_collector_id();

    std::atomic<int> active{0};
    std::mutex snap_lock;

    std::mutex all_buffers_lock;
    std::vector<std::unique_ptr<ThreadBuffers> > all_buffers;

    std::array<uint64_t, 256> global_buckets_{};
    uint64_t global_count_ = 0;
    uint64_t global_sum_ = 0;
    uint64_t global_min_ = UINT64_MAX;
    uint64_t global_max_ = 0;
};


#endif //ITMO_PARALLELPROG_DOUBLEBUFFERCOLLECTOR_H
