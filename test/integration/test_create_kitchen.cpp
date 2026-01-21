#include <gtest/gtest.h>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

#include "Reception.hpp"

TEST(Integration, ForkAndPipeWork) {
    // Redirect stdin
    std::istringstream input("Regina S x1\nexit\n");
    std::cin.rdbuf(input.rdbuf());

    // Capture stdout
    testing::internal::CaptureStdout();

    Reception reception(1.0f, 1, 1);
    reception.run();

    std::string output = testing::internal::GetCapturedStdout();

    // Assertions on observable behavior
    EXPECT_NE(output.find("Kitchen running!"), std::string::npos);
    EXPECT_NE(output.find("Order sent:"), std::string::npos);
    EXPECT_NE(output.find("Order received:"), std::string::npos);
    EXPECT_NE(output.find("Exitting..."), std::string::npos);
}
