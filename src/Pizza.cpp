#include "Pizza.hpp"
#include "utils/utils.hpp"

PizzaType Pizza::parsePizzaType(std::string input) {
    PizzaType type;

    input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
    if (input == "Regina")
        type = PizzaType::Regina;
    else if (input == "Margarita")
        type = PizzaType::Margarita;
    else if (input == "Americana")
        type = PizzaType::Americana;
    else if (input == "Fantasia")
        type = PizzaType::Fantasia;
    else {
        type = PizzaType::Error;
        std::cout << ERR_INVALID_PIZZA_NAME << std::endl;
    }
    return type;
}

PizzaSize Pizza::parsePizzaSize(std::string input) {
    PizzaSize size;

    input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
    if (input == "S")
        size = PizzaSize::S;
    else if (input == "M")
        size = PizzaSize::M;
    else if (input == "L")
        size = PizzaSize::L;
    else if (input == "XL")
        size = PizzaSize::XL;
    else if (input == "XXL")
        size = PizzaSize::XXL;
    else {
        size = PizzaSize::Error;
        std::cout << ERR_INVALID_PIZZA_SIZE << std::endl;
    }
    return size;
}

int Pizza::parsePizzaAmount(std::string input) {
    int amount;

    input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
    if (input[0] == 'x')
        input.erase(0, 1);
    else {
        std::cout << ERR_INVALID_PIZZA_AMOUNT << std::endl;        
        return -1;
    }
    try {
        if (isFloat(input))
            return -1;
        amount = std::stoi(input);
    } catch (std::exception &e) {
        std::cout << ERR_INVALID_PIZZA_AMOUNT << std::endl; 
        return -1;
    }
    return amount;
}