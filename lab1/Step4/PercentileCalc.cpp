#include "PercentileCalc.hpp"


uint64_t computePercentile(const std::array<uint64_t, 256> &buckets,
                           uint64_t count, double p) {
    if (count == 0) return 0;
    double threshold = count * p;
    uint64_t accumulated = 0;
    for (size_t i = 0; i < 256; ++i) {
        accumulated += buckets[i];
        if (accumulated >= threshold)
            return i * 4;
    }
    return 255 * 4;
}
