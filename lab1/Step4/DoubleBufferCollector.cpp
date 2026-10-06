//
// Created by ivan on 06.10.2026.
//

#include "DoubleBufferCollector.h"
#include "PercentileCalc.hpp"

#include <thread>

void DoubleBufferCollector::record(uint64_t value) {
    ThreadBuffers *my = get_my_buffers();
    int b;

    while (true) {
        b = active.load(std::memory_order_seq_cst);
        my->inside.store(b, std::memory_order_seq_cst);
        if (active.load(std::memory_order_seq_cst) == b) {
            break;
        }
        my->inside.store(-1, std::memory_order_seq_cst);
    }

    uint64_t bucket_idx = std::min(value / 4, static_cast<uint64_t>(255));
    my->buf[b].buckets[bucket_idx]++;
    my->buf[b].count++;
    my->buf[b].sum += value;
    if (value < my->buf[b].min) my->buf[b].min = value;
    if (value > my->buf[b].max) my->buf[b].max = value;
    my->inside.store(-1, std::memory_order_release);
}

Snapshot DoubleBufferCollector::snapshot() {
    std::lock_guard<std::mutex> g(snap_lock);
    int old_active = active.load(std::memory_order_seq_cst);
    active.store(1 - old_active, std::memory_order_seq_cst);

    for (auto& tb: all_buffers) {
        while (tb->inside.load(std::memory_order_seq_cst) == old_active) {
            std::this_thread::yield();
        }
    }

    for (auto& tb: all_buffers) {
        Buf &b = tb->buf[old_active];
        for (size_t i = 0; i < 256; ++i)
            global_buckets_[i] += b.buckets[i];
        global_count_ += b.count;
        global_sum_ += b.sum;
        global_min_ = std::min(global_min_, b.min);
        global_max_ = std::max(global_max_, b.max);

        b.clear();
    }

    Snapshot s;
    s.buckets = global_buckets_;   // копия
    s.count = global_count_;
    s.sum   = global_sum_;
    s.min   = (global_count_ == 0) ? 0 : global_min_;
    s.max   = global_max_;
    s.p50 = computePercentile(s.buckets, s.count, 0.50);
    s.p99 = computePercentile(s.buckets, s.count, 0.99);
    return s;
}

ThreadBuffers *DoubleBufferCollector::get_my_buffers() {
    struct TLSSlot {
        uint64_t id = 0;
        ThreadBuffers *bufs = nullptr;
    };
    static thread_local TLSSlot slot;

    if (slot.id != id) {
        auto tb = std::make_unique<ThreadBuffers>();
        ThreadBuffers *raw = tb.get();
        {
            std::lock_guard<std::mutex> g(snap_lock);
            all_buffers.push_back(std::move(tb));
        }
        slot.id = id;
        slot.bufs = raw;
    }
    return slot.bufs;
}
