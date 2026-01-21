#include "gtest/gtest.h"
#include "Kitchen.hpp"

TEST(KitchenIntegration, ReadsOrdersAndStopsOnEOF) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    testing::internal::CaptureStdout();

    pid_t pid = fork();
    ASSERT_NE(pid, -1);

    if (pid == 0) {
        close(pipefd[1]);
        Kitchen kitchen(1.0f, 1, 1);
        kitchen.run(pipefd);
        _exit(0);
    }

    close(pipefd[0]);

    write(pipefd[1], "1 1 1\n", 6);
    write(pipefd[1], "2 2 3\n", 6);
    close(pipefd[1]);

    waitpid(pid, nullptr, 0);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Kitchen running!"), std::string::npos);
    EXPECT_NE(output.find("Order received: 1 1 1"), std::string::npos);
    EXPECT_NE(output.find("Order received: 2 2 3"), std::string::npos);
    EXPECT_NE(output.find("Pipe closed, kitchen shutting down."), std::string::npos);
}
