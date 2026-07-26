#ifndef RECEPTION_HPP
#define RECEPTION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <mutex>

#include "Pizza.hpp"
#include "Kitchen.hpp"
#include "KitchenStatus.hpp"

#define ERR_INVALID_INPUT   "Error: invalid input."
#define ERR_FAILED_FORK     "Error: failed to spawn a child."

#define CHILD_PID 0

struct KitchenInfo {
    int pid;
    int pipefd[2];
    KitchenStatus* status;
};

class Reception {
public:
    Reception(float speed_multi, int cook_nb, int restock_timer);
    ~Reception();

    int run(void);
    void shutdown(void);

    void displayStatus(void) const;

    int handleNewOrders(void);
    int createNewKitchen(void);
    KitchenStatus* allocSharedStatus(void);
    void freeSharedStatus(KitchenInfo* info);
    bool sendOrderToKitchen(PizzaOrder order, KitchenInfo *k) const;

    int getInput(void);
    int parseInput(std::string input);
    int parseOrder(std::string line);

    std::vector<PizzaOrder> getNewOrders(void) {
        std::lock_guard<std::mutex> lock(status_mtx);
        return this->newOrders;
    }
    std::vector<PizzaOrder> getPendingOrders(void) {
        std::lock_guard<std::mutex> lock(status_mtx);
        return this->pendingOrders;
    }
    float getSpeedMultiplier(void) const { return speed_multiplier; }
    int getCookNb(void) const { return cook_nb; }
    int getRestockTimer(void) const { return restock_timer; }
    std::vector<KitchenInfo *> getKitchens(void) {
        std::lock_guard<std::mutex> lock(status_mtx);
        return this->kitchens;
    }

private:
    float speed_multiplier;
    int cook_nb;
    int restock_timer;

    std::vector<KitchenInfo *> kitchens;

    std::vector<PizzaOrder> newOrders;
    std::vector<PizzaOrder> pendingOrders;

    mutable std::mutex status_mtx;
};

#endif //RECEPTION_HPP