#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <latch>
#include <thread>
#include <vector>

#include "DoubleBufferCollector.h"
#include "Generator.hpp"
#include "MetricsCollector.hpp"
#include "PercentileCalc.hpp"

int main() {
    constexpr uint64_t SEED = 42;
    constexpr size_t N = 1u << 20;
    constexpr int NUM_WRITERS = 4;
    constexpr int NUM_SNAPSHOTS = 10'000;

    auto values = generate(N, SEED);

    DoubleBufferCollector collector;

    std::atomic<bool> stop{false};
    std::latch start(1);
    std::vector<uint64_t> per_thread(NUM_WRITERS, 0);
    std::vector<std::thread> writers;

    for (int k = 0; k < NUM_WRITERS; ++k) {
        writers.emplace_back([&, k] {
            uint64_t local = 0;
            size_t i = k * 1000;
            start.wait();
            while (!stop.load(std::memory_order_relaxed)) {
                collector.record(values[i]);
                local++;
                if (++i == values.size()) i = 0;
            }
            per_thread[k] = local;
        });
    }

    start.count_down();

    size_t broken = 0, less = 0, greater = 0;
    Snapshot s;
    for (int iter = 0; iter < NUM_SNAPSHOTS; ++iter) {
        s = collector.snapshot();

        uint64_t sum_buckets = 0;
        for (auto b: s.buckets) sum_buckets += b;

        if (sum_buckets != s.count) {
            ++broken;
            if (sum_buckets < s.count) ++less;
            else ++greater;
        }
    }

    stop.store(true, std::memory_order_relaxed);
    for (auto &t: writers) t.join();

    Snapshot final_snap = collector.snapshot();

    uint64_t expected = 0;
    for (auto x: per_thread) expected += x;

    std::cout << "writers:          " << NUM_WRITERS << "\n";
    std::cout << "snapshots taken:  " << NUM_SNAPSHOTS << "\n";
    std::cout << "sum by threads:   " << expected << "\n";
    std::cout << "final snap count: " << final_snap.count << "\n";
    std::cout << "broken snapshots: " << broken
            << " (" << (100.0 * broken / NUM_SNAPSHOTS) << "%)\n";
    std::cout << "sum greater:      " << greater << "\n"
            << "sum less:         " << less << "\n";
    return 0;
}
