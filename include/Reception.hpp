#ifndef RECEPTION_HPP
#define RECEPTION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "Pizza.hpp"
#include "Kitchen.hpp"

#define ERR_INVALID_INPUT   "Error: invalid input."
#define ERR_FAILED_FORK     "Error: failed to spawn a child."

#define CHILD_PID 0

struct KitchenInfo {
    int pid;
    int pipefd[2];
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
    bool sendOrderToKitchen(PizzaOrder order, KitchenInfo *k) const;

    int getInput(void);
    int parseInput(std::string input);
    int parseOrder(std::string line);

    std::vector<PizzaOrder> getNewOrders(void) {return this->newOrders;};
    std::vector<PizzaOrder> getPendingOrders(void) {return this->pendingOrders;};

private:
    float speed_multiplier;
    int cook_nb;
    int restock_timer;

    std::vector<KitchenInfo *> kitchens;

    std::vector<PizzaOrder> newOrders;
    std::vector<PizzaOrder> pendingOrders;
};

#endif //RECEPTION_HPP