#ifndef PIZZA_HPP
#define PIZZA_HPP

#include <sstream>
#include <iostream>
#include <algorithm>
#include <exception>

#define ERR_INVALID_PIZZA_NAME "Error: Invalid pizza name."
#define ERR_INVALID_PIZZA_SIZE "Error: Invalid pizza size."
#define ERR_INVALID_PIZZA_AMOUNT "Error: Invalid pizza amount."

enum class PizzaType {
    Error = 0,
    Regina = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia = 8
};

enum class PizzaSize {
    Error = 0,
    S = 1,
    M = 2,
    L = 4,
    XL = 8,
    XXL = 16
};

struct PizzaOrder {
    PizzaType type;
    PizzaSize size;
    int amount;
};

inline constexpr const char *PizzaNames[] = { "Regina", "Margarita", "Americana", "Fantasia" };
inline constexpr const char *PizzaSizes[] = { "S", "M", "L", "XL", "XXL" };

class Pizza {
public:
    static PizzaType parseType(std::string input);
    static PizzaSize parseSize(std::string input);
    static int parseAmount(std::string input);
    static PizzaOrder parseOrder(std::string input);

    static std::string orderToString(PizzaOrder order);
    static std::string typeToString(PizzaType type);
    static std::string sizeToString(PizzaSize size);
    static std::string amountToString(int amount);
};

#endif // PIZZA_HPP