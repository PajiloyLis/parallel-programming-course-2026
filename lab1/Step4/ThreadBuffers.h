#ifndef ITMO_PARALLELPROG_THREADBUFFERS_H
#define ITMO_PARALLELPROG_THREADBUFFERS_H
#include <array>
#include <atomic>

constexpr int NOWHERE = -1;


struct Buf {
    std::array<uint64_t, 256> buckets{};
    uint64_t count = 0, sum = 0, min = UINT64_MAX, max = 0;

    void clear() {
        buckets.fill(0);
        count = 0;
        sum = 0;
        min = UINT64_MAX;
        max = 0;
    }
};

struct alignas(64) ThreadBuffers {
    std::atomic<int> inside{NOWHERE};
    Buf buf[2];
};

#endif //ITMO_PARALLELPROG_THREADBUFFERS_H
