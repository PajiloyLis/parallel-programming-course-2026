#ifndef ITMO_PARALLELPROG_PERCENTILECALC_H
#define ITMO_PARALLELPROG_PERCENTILECALC_H

#include <array>
#include <cstdint>
#include <cstddef>

uint64_t computePercentile(const std::array<uint64_t, 256> &buckets,
                           uint64_t count, double p);

#endif //ITMO_PARALLELPROG_PERCENTILECALC_H
