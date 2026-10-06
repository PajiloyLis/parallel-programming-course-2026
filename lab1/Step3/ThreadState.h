#ifndef ITMO_PARALLELPROG_THREADSTATE_H
#define ITMO_PARALLELPROG_THREADSTATE_H
#include <array>
#include <atomic>

struct alignas(64) ThreadState {
    std::array<std::atomic_uint64_t, 256> buckets{};
    std::atomic_uint64_t count{0};
    std::atomic_uint64_t sum{0};
    std::atomic_uint64_t min{UINT64_MAX};
    std::atomic_uint64_t max{0};
};

#endif //ITMO_PARALLELPROG_THREADSTATE_H