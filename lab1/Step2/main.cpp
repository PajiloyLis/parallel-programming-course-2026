#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <latch>
#include <thread>
#include <vector>

#include "Generator.hpp"
#include "MetricsCollector.hpp"
#include "ShardedCollector.hpp"

double run(MetricsCollector &collector,
           const std::vector<uint64_t> &values,
           int T, double seconds) {
    std::latch start(1);
    std::atomic<bool> stop{false};
    std::vector<uint64_t> ops(T, 0);
    std::vector<std::thread> threads;

    for (int k = 0; k < T; ++k) {
        threads.emplace_back([&, k] {
            uint64_t local = 0;
            size_t i = k * 1000;
            start.wait();
            while (!stop.load(std::memory_order_relaxed)) {
                collector.record(values[i]);
                local++;
                if (++i == values.size()) i = 0;
            }
            ops[k] = local;
        });
    }

    auto t0 = std::chrono::steady_clock::now();
    start.count_down();
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    stop.store(true, std::memory_order_relaxed);

    for (auto &t: threads) t.join();
    auto t1 = std::chrono::steady_clock::now();

    uint64_t total = 0;
    for (auto x: ops) total += x;
    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    return total / elapsed;
}

double measurePoint(MetricsCollector &collector,
                    const std::vector<uint64_t> &values,
                    int T) {
    run(collector, values, T, 5.0);

    std::vector<double> results;
    results.reserve(5);
    for (int i = 0; i < 5; ++i)
        results.push_back(run(collector, values, T, 5.0));

    const auto snap = collector.snapshot();
    std::cerr << "  count = " << snap.count
            << "  p50 = " << snap.p50
            << "  p99 = " << snap.p99 << "\n";

    std::sort(results.begin(), results.end());
    return results[results.size() / 2];
}

int main(int argc, char **argv) {

    std::unique_ptr<MetricsCollector> collector = std::make_unique<ShardedCollector>();

    constexpr uint64_t SEED = 42;
    constexpr size_t N = 1u << 20;

    auto values = generate(N, SEED);
    std::cout << "# values generated: " << values.size() << "\n";

    const std::vector<int> thread_counts = {1, 2, 4, 8, 16};

    std::cout << "# T\tops/sec\n";
    for (int T: thread_counts) {
        double ops = measurePoint(*collector, values, T);
        std::cout << T << "\t" << static_cast<uint64_t>(ops) << "\n";
    }
    return 0;
}
