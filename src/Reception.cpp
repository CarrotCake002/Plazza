#include "Reception.hpp"

Reception::Reception(float speed_multi, int cook_nb, int restock_timer) : speed_multiplier(speed_multi), cook_nb(cook_nb), restock_timer(restock_timer) {

}

Reception::~Reception() {

}

void Reception::displayStatus(void) {
    std::cout << "status" << std::endl;
}

int Reception::parseOrder(std::string line) {
    std::istringstream iss(line);
    std::string pizzaType;
    std::string pizzaSize;
    std::string pizzaAmount;
    std::string leftover;
    PizzaOrder order;

    iss >> pizzaType >> pizzaSize >> pizzaAmount >> leftover;
    if (!leftover.empty() || pizzaType.empty() || pizzaSize.empty() || pizzaAmount.empty())
        return 1;
    order = {
        Pizza::parsePizzaType(pizzaType),
        Pizza::parsePizzaSize(pizzaSize),
        Pizza::parsePizzaAmount(pizzaAmount)
    };
    if (order.type == PizzaType::Error || order.size == PizzaSize::Error || order.amount < 1)
        return 1;
    this->newOrders.push_back(order);
    return 0;
}

int Reception::parseInput(std::string input) {
    std::istringstream iss(input);
    std::string line;

    while (std::getline(iss, line, ';')) {
        if (parseOrder(line) == 1) {
            this->newOrders.clear();
            return 1;
        }
    }
    return 0;
}

int Reception::getInput(void) {
    std::string input;

    std::cout << " > ";
    std::getline(std::cin, input);
    if (input == "exit")
        return 1;
    if (input == "status") {
        displayStatus();
        return 0;
    }
    if (parseInput(input) == 1)
        return 0;
    return 0;
}

int Reception::handleNewOrders(void) {
    std::cout << "There's " << this->newOrders.size() << " new orders!" << std::endl;
    return 0;
}

int Reception::run(void) {
    int status = true;

    while (status) {
        if (getInput() == 1) {
            std::cout << "Exitting..." << std::endl;
            break;
        }
        if (handleNewOrders() < 0)
            return -1;
    }
    return 0;
}