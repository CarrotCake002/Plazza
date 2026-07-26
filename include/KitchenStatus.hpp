#ifndef KITCHEN_STATUS_HPP
#define KITCHEN_STATUS_HPP

#include <atomic>

struct KitchenStatus {
    std::atomic<int> orders_queued{0};
    std::atomic<int> orders_cooking{0};
    std::atomic<int> cooks_total{0};
};

#endif
