#ifndef ITMO_PARALLELPROG_METRICSCOLLECTOR_H
#define ITMO_PARALLELPROG_METRICSCOLLECTOR_H

#include <cstdint>

#include "Snapshot.hpp"

class MetricsCollector {
public:
    virtual ~MetricsCollector() = default;

    virtual void record(uint64_t value) = 0;

    virtual Snapshot snapshot() = 0;
};

#endif //ITMO_PARALLELPROG_METRICSCOLLECTOR_H
