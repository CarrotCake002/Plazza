#include "Reception.hpp"

Reception::Reception(float speed_multi, int cook_nb, int restock_timer) : speed_multiplier(speed_multi), cook_nb(cook_nb), restock_timer(restock_timer) {

}

Reception::~Reception() {

}

void Reception::shutdown(void) {
    KitchenInfo *info;

    for (auto it = kitchens.begin(); it != kitchens.end(); it++) {
        info = *it;
        close(info->pipefd[1]);
        waitpid(info->pid, nullptr, 0);
        delete info;
    }
    kitchens.clear();
    std::cout << "Exitting..." << std::endl;
}

void Reception::displayStatus(void) const {
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

std::string Reception::convertOrderToString(PizzaOrder order) const {
    std::string str_order = std::to_string(static_cast<int>(order.type)) + " "
                        + std::to_string(static_cast<int>(order.size)) + " "
                        + std::to_string(static_cast<int>(order.amount));
    return str_order;
}

bool Reception::sendOrderToKitchen(PizzaOrder order, KitchenInfo *k) const {
    std::string value = convertOrderToString(order);

    std::cout << "Order sent: " << value << std::endl;
    std::cout << "Order size: " << value.size() << std::endl;
    value += '\n';
    write(k->pipefd[1], value.c_str(), value.size() + 1);
    return true;
}

int Reception::createNewKitchen(void) {
    pid_t pid;
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }
    
    pid = fork();
    
    if (pid == -1) {
        std::cout << ERR_FAILED_FORK << std::endl;
        return -1;
    } else if (pid == CHILD_PID) {  // Child process
        for (KitchenInfo *k : kitchens) {
            close(k->pipefd[0]);
            close(k->pipefd[1]);
        }
        close(pipefd[1]);           // Close writing

        Kitchen kitchen = Kitchen(speed_multiplier, cook_nb, restock_timer);

        kitchen.run(pipefd);
        exit(EXIT_SUCCESS);
    }
    // Parent process
    close(pipefd[0]);           // Close reading

    KitchenInfo *info = new KitchenInfo();

    info->pid = pid;
    info->pipefd[0] = 0;
    info->pipefd[1] = pipefd[1];

    std::cout << "Adding new kitchen info in parent!" << std::endl;
    kitchens.push_back(info);
    std::cout << "New kitchen pid: " << kitchens[kitchens.size() - 1]->pid << std::endl;
    return 0;
}

int Reception::handleNewOrders(void) {
    int status = createNewKitchen();
    std::vector<PizzaOrder>::iterator curr_order;
    
    if (status < 0) {
        std::cout << "Error: failed to create kitchen." << std::endl;    
        return -1;
    }

    std::cout << "There's " << this->newOrders.size() << " new orders!" << std::endl;

    auto it = newOrders.begin();

    while (it != newOrders.end()) {
        KitchenInfo *kitchen = kitchens.back();
        sendOrderToKitchen(*it, kitchen);

        // Move new order to pending orders
        pendingOrders.push_back(*it);

        // Remove order from new orders
        it = newOrders.erase(it);
    }

    std::cout << "Orders have been assigned!" << std::endl;
    std::cout << "There's " << this->newOrders.size() << " new orders!" << std::endl;
    std::cout << "There's " << this->pendingOrders.size() << " pending orders!" << std::endl;
    return 0;
}

int Reception::run(void) {
    int status = true;

    while (status) {
        if (getInput() == 1) {
            shutdown();
            break;
        }
        if (newOrders.size() > 0) {
            if (handleNewOrders() < 0)
                return -1;
        }
    }
    return 0;
}