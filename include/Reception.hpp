#ifndef RECEPTION_HPP
#define RECEPTION_HPP

#include <iostream>
#include <vector>
#include <sstream>

#include "Pizza.hpp"

class Reception {
public:
    Reception(float speed_multi, int cook_nb, int restock_timer);
    ~Reception();

    int run(void);

    void displayStatus(void);

    int getInput(void);
    int parseInput(std::string input);

private:
    float speed_multiplier;
    int cook_nb;
    int restock_timer;

    std::vector<PizzaOrder> newOrders;
    std::vector<PizzaOrder> pendingOrders;
};

#endif //RECEPTION_HPP