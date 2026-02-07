#define UNIT_TEST
#include "gtest/gtest.h"
#include "Kitchen.hpp"

#include <thread>
#include <chrono>
#include <unistd.h>
#include <cstring>

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

    write(pipefd[1], "Regina S x1\n", 13);
    write(pipefd[1], "Americana L x3\n", 16);
    close(pipefd[1]);

    waitpid(pid, nullptr, 0);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Kitchen running!"), std::string::npos);
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

    write(pipeA[1], "Regina S x1\n", 6);
    write(pipeB[1], "Americana L x2\n", 6);

    close(pipeA[1]);
    close(pipeB[1]);

    waitpid(pidA, nullptr, 0);
    waitpid(pidB, nullptr, 0);

    std::string output = testing::internal::GetCapturedStdout();

    // Assertions
    EXPECT_EQ(std::count(output.begin(), output.end(), '\n') >= 2, true);
    EXPECT_EQ(
        std::count(
            output.begin(), output.end(),
            std::string("Pipe closed, kitchen shutting down.")[0]
        ) >= 2,
        true
    );
}

/* =========================================================
   addOrderToList unit-level tests
   ========================================================= */

TEST(KitchenUnit, AddOrderSingle)
{
    Kitchen kitchen(1.0f, 1, 1);

    kitchen.addOrderToList("Regina S x1\n");

    EXPECT_EQ(kitchen.getOrderCount(), 1);
}

TEST(KitchenUnit, AddOrderMultipleAmount)
{
    Kitchen kitchen(1.0f, 1, 1);

    kitchen.addOrderToList("Americana L x3\n");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(kitchen.getPendingCount(), 3);
}

TEST(KitchenUnit, AddMultipleOrdersInOneChunk)
{
    Kitchen kitchen(1.0f, 1, 1);

    kitchen.addOrderToList("Margarita XXL x3\n");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(kitchen.getPendingCount(), 3);
}

/* =========================================================
   Thread lifecycle / destructor safety
   ========================================================= */

TEST(KitchenLifecycle, ConstructorDestructorNoDeadlock)
{
    {
        Kitchen kitchen(1.0f, 2, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SUCCEED(); // destructor joined all threads
}

/* =========================================================
   Condition variable / cook behavior
   ========================================================= */

TEST(KitchenConcurrency, CookConsumesOneOrder)
{
    Kitchen kitchen(1.0f, 1, 1);

    kitchen.addOrderToList("Regina S x1\n");
    kitchen.cv.notify_one();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(kitchen.getOrderCount(), 0);
}

TEST(KitchenConcurrency, CookConsumesMultipleOrders)
{
    Kitchen kitchen(1.0f, 1, 1);

    kitchen.addOrderToList("Regina S x3\n");
    kitchen.cv.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(kitchen.getOrderCount(), 0);
}

/* =========================================================
   run() pipe behavior (single-process, no fork)
   ========================================================= */

TEST(KitchenRun, PipeCreatesOrders)
{
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    Kitchen kitchen(1.0f, 1, 1);

    std::thread runner([&]() {
        kitchen.run(pipefd);
    });

    write(pipefd[1], "Regina S x2\n", 13);
    close(pipefd[1]);

    runner.join();

    kitchen.displayAllOrders();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(kitchen.getPendingCount(), 2);
}
