#include "Plazza.hpp"

int run(float speed_multi, int cook_nb, int restock_timer) {
    Reception reception(speed_multi, cook_nb, restock_timer);

    return reception.run();
}

int main(int ac, char **av) {
    int status = errorHandling(ac, (const char**)av);

    if (status == 1)
        return 0;
    else if (status == -1)
        return 84;
    return run(std::stof(av[1]), std::stoi(av[2]), std::stoi(av[3]));
}
