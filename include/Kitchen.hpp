#ifndef KITCHEN_HPP
#define KITCHEN_HPP

#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Pizza.hpp"

#define PIPE_MESSAGE_SIZE 128

#define ERROR_FAILED_READ_ORDER "Error: failed to read order."

class Kitchen {
public:
    Kitchen(float speed_multip, int cooks_nb, int restock_timer);
    ~Kitchen(void);

    void run(int *pipefd);
    int run_cook(void);

    void addOrderToList(std::string pipe_str);

    void displayAllOrders(void);
    
    std::condition_variable cv;

#ifdef UNIT_TEST
public:
    size_t getOrderCount() const { // change to pending orders not normal orders
        std::lock_guard<std::mutex> lock(mtx);
        return orders.size();
    }
    size_t getPendingCount() const { // change to pending orders not normal orders
        std::lock_guard<std::mutex> lock(mtx);
        return pending.size();
    }

    bool isShutdown() const {
        return shutdown.load();
    }
#endif

private:
    float speed_multip;
    int cooks_nb;
    int restock_timer;

    mutable std::mutex mtx;

    std::vector<PizzaOrder> orders;
    std::vector<PizzaOrder> pending;
    std::vector<std::thread> cooks;
    std::atomic<bool> shutdown{false};
};

#endif // KITCHEN_HPP