#ifndef ITMO_PARALLELPROG_SNAPSHOT_H
#define ITMO_PARALLELPROG_SNAPSHOT_H

#include <array>
#include <cstdint>

struct Snapshot {
    std::array<uint64_t, 256> buckets;
    uint64_t count;
    uint64_t sum;
    uint64_t min;
    uint64_t max;
    uint64_t p50;
    uint64_t p99;
};

#endif //ITMO_PARALLELPROG_SNAPSHOT_H
