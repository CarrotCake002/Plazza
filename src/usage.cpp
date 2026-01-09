#include "usage.hpp"

bool isPositive(int nb) {
    return nb > 0 ? true : false;
}

int checkArguments(int speed_multi, int cook_nb, int restock_timer) {
    if (speed_multi <= 0 || cook_nb <= 0 || restock_timer <= 0) {
        std::cout << ERROR_INVALID_ARG_VALUE << std::endl;
        return false;
    }
    return true;
}

int errorHandling(int ac, char **av) {

    if (ac == 2 && (std::string(av[1]) == "-h" || std::string(av[1]) == "--help")) {
        std::cout << HELP << std::endl;
        return 1;
    }
    if (ac != 4) {
        std::cout << ERROR_INVALID_ARG_NB << std::endl;
        return -1;
    }
    try {
        if (!checkArguments(std::stoi(av[1]), std::stoi(av[2]), std::stoi(av[3])))
            return -1;
    } catch (std::exception &e) {
        std::cout << ERROR_INVALID_ARG << std::endl;
        return -1;
    }
    return 0;
}
