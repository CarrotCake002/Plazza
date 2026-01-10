#include "Plazza.hpp"

int main(int ac, char **av) {
    int status = errorHandling(ac, (const char**)av);

    if (status == 1)
        return 0;
    else if (status == -1)
        return 84;
    std::cout << "Hello world!" << std::endl;
    return 0;
}
