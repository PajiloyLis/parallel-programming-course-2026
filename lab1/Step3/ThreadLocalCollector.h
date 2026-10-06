#ifndef ITMO_PARALLELPROG_THREADLOCALCOLLECTOR_H
#define ITMO_PARALLELPROG_THREADLOCALCOLLECTOR_H
#include <atomic>
#include <memory>
#include <vector>

#include "MetricsCollector.hpp"
#include "ThreadState.h"

inline uint64_t next_collector_id() {
    static std::atomic<uint64_t> counter{1};
    return counter.fetch_add(1);
}

class ThreadLocalCollector : public MetricsCollector {
public:

    void record(uint64_t value) override;

    Snapshot snapshot() override;

private:
    const uint64_t id = next_collector_id();
    std::mutex list_lock;
    std::vector<std::unique_ptr<ThreadState> > all_states;

    ThreadState *get_my_state();
};

#endif //ITMO_PARALLELPROG_THREADLOCALCOLLECTOR_H
