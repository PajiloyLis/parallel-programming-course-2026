#include "Generator.hpp"

std::vector<uint64_t> generate(const size_t n, const uint64_t seed) {
    constexpr double exponent = 1.15;
    constexpr int K = 1023;

    std::vector<double> weights(K + 1);
    for (int k = 1; k <= K; ++k)
        weights[k] = 1.0 / std::pow(k, exponent);

    std::mt19937_64 rnd(seed);
    std::discrete_distribution dist(weights.begin(), weights.end());

    std::vector<uint64_t> values(n);
    for (auto& v : values)
        v = dist(rnd);
    return values;
}
