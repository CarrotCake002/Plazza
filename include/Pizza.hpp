#ifndef PIZZA_HPP
#define PIZZA_HPP

#include <map>

enum PizzaType {
    Regina = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia = 8
};

enum PizzaSize {
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

#endif // PIZZA_HPP