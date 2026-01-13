#include "Reception.hpp"

Reception::Reception(float speed_multi, int cook_nb, int restock_timer) : speed_multiplier(speed_multi), cook_nb(cook_nb), restock_timer(restock_timer) {

}

Reception::~Reception() {

}

int Reception::parseInput(std::string input) {
    std::istringstream iss(input);
    std::string pizzaType;
    std::string pizzaSize;
    std::string pizzaAmount;

    iss >> pizzaType >> pizzaSize >> pizzaAmount;
    return 0;
}

void Reception::displayStatus(void) {
    std::cout << "status" << std::endl;
}

int Reception::getInput(void) {
    std::string input;
    PizzaOrder order;

    std::cout << " > ";
    std::getline(std::cin, input);
    if (input == "exit")
        return 1;
    if (input == "status") {
        displayStatus();
        return 0;
    }
    parseInput(input);
    return 0;
}

int Reception::run(void) {
    int status = true;

    while (status) {
        if (this->getInput() == 1) {
            std::cout << "Exitting..." << std::endl;
            break;
        }
    }
    return 0;
}