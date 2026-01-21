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

TEST(KitchenIntegration, TwoKitchensCloseIndependently) {
    int pipeA[2], pipeB[2];
    ASSERT_EQ(pipe(pipeA), 0);
    ASSERT_EQ(pipe(pipeB), 0);

    testing::internal::CaptureStdout();

    pid_t pidA = fork();
    ASSERT_NE(pidA, -1);

    if (pidA == 0) {
        close(pipeA[1]);
        Kitchen kitchenA(1.0f, 1, 1);
        kitchenA.run(pipeA);
        _exit(0);
    }

    pid_t pidB = fork();
    ASSERT_NE(pidB, -1);
    
    if (pidB == 0) {
        close(pipeA[0]);
        close(pipeA[1]);
        close(pipeB[1]);
        Kitchen kitchenB(1.0f, 1, 1);
        kitchenB.run(pipeB);
        _exit(0);
    }

    // Parent
    close(pipeA[0]);
    close(pipeB[0]);

    write(pipeA[1], "1 1 1\n", 6);
    write(pipeB[1], "2 2 2\n", 6);

    close(pipeA[1]);
    close(pipeB[1]);

    waitpid(pidA, nullptr, 0);
    waitpid(pidB, nullptr, 0);

    std::string output = testing::internal::GetCapturedStdout();

    // Assertions
    EXPECT_EQ(std::count(output.begin(), output.end(), '\n') >= 2, true);
    EXPECT_NE(output.find("Order received: 1 1 1"), std::string::npos);
    EXPECT_NE(output.find("Order received: 2 2 2"), std::string::npos);
    EXPECT_EQ(
        std::count(
            output.begin(), output.end(),
            std::string("Pipe closed, kitchen shutting down.")[0]
        ) >= 2,
        true
    );
}
