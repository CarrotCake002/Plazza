#include "Kitchen.hpp"

Kitchen::Kitchen(float speed_multip, int cooks_nb, int restock_timer) : speed_multip(speed_multip), cooks_nb(cooks_nb), restock_timer(restock_timer) {
    std::cout << "Kitchen created!" << std::endl;

    for (int i = 0; i < cooks_nb; i++) {
        cooks.push_back(std::thread(&Kitchen::run_cook, this));
    }
}

Kitchen::~Kitchen(void) {
    std::cout << "Kitchen closed!" << std::endl;
    //shutdown = true;
    for (auto &cook : cooks) {
        if (cook.joinable())
            cook.join();
    }
}

int Kitchen::run_cook(void) {
    std::cout << "Cook is running!" << std::endl;

    
    
    while (!this->shutdown) {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv.wait(lock, [&] {
            return shutdown || !orders.empty();
        });
        std::cout << "Cooking " << Pizza::orderToString(orders.back()) << "..." << std::endl;
        orders.pop_back(); // instead of popping, add to a cooking queue or smth
    }
    return 0;
}

void Kitchen::addOrderToList(std::string pipe_str) {
    ssize_t pos;
    std::string line;

    while ((long unsigned int)(pos = pipe_str.find('\n')) != std::string::npos) {
        line = pipe_str.substr(0, pos);
        pipe_str.erase(0, pos + 1);

        if (!line.empty()) {
            std::cout << "Order received: " << line << std::endl;
        }
    }

    PizzaOrder order = Pizza::parseOrder(line);
    std::unique_lock lock(mtx);

    for (int i = 0; i < order.amount; i++) {
        orders.push_back((PizzaOrder){order.type, order.size, 1});
    }
}

void Kitchen::run(int *pipefd) {
    //int status = true;
    std::cout << "Kitchen running!" << std::endl;
    char buffer[PIPE_MESSAGE_SIZE];
    std::string pending;
    ssize_t bytesRead;

    while ((bytesRead = read(pipefd[0], &buffer, PIPE_MESSAGE_SIZE)), bytesRead > 0) {
        pending.append(buffer, bytesRead);
        std::cout << "Size read: " << bytesRead << std::endl;

        addOrderToList(pending);

        std::cout << "Notifying!" << std::endl;
        cv.notify_one();
    }
    
    std::cout << "Size of message received: " << bytesRead << std::endl;
    if (bytesRead < 0) {
        std::cout << ERROR_FAILED_READ_ORDER << std::endl;
    }
    std::cout << "Pipe closed, kitchen shutting down." << std::endl;
    close(pipefd[0]);
}