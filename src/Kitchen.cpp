#include "Kitchen.hpp"

Kitchen::Kitchen(float speed_multip, int cooks_nb, int restock_timer) : speed_multip(speed_multip), cooks_nb(cooks_nb), restock_timer(restock_timer) {
    std::cout << "Kitchen created!" << std::endl;
}

Kitchen::~Kitchen(void) {
    std::cout << "Kitchen closed!" << std::endl;
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

        ssize_t pos;
        while ((long unsigned int)(pos = pending.find('\n')) != std::string::npos) {
            std::string line = pending.substr(0, pos);
            pending.erase(0, pos + 1);

            if (!line.empty()) {
                std::cout << "Order received: " << line << std::endl;
            }
        }
    }
    
    std::cout << "Size of message received: " << bytesRead << std::endl;
    if (bytesRead < 0) {
        std::cout << ERROR_FAILED_READ_ORDER << std::endl;
    }
    std::cout << "Pipe closed, kitchen shutting down." << std::endl;
    close(pipefd[0]);
}