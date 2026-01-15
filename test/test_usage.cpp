#include "usage.hpp"
#include "gtest/gtest.h"

TEST(checkArguments, HandlesCorrectValues) {
    EXPECT_EQ(checkArguments(3, 5, 2000), true);
    EXPECT_EQ(checkArguments(1000, 5, 1), true);
    EXPECT_EQ(checkArguments(0.1, 1, 10), true);
    EXPECT_EQ(checkArguments(0.0001, 1, 10), true);
}

TEST(checkArguments, HandlesIncorrectValues) {
    EXPECT_EQ(checkArguments(0, 5, 2000), false);
    EXPECT_EQ(checkArguments(0.96, -1, 2000), false);
    EXPECT_EQ(checkArguments(10, 5, 0), false);
    EXPECT_EQ(checkArguments(-0.4, 5, 100), false);
    EXPECT_EQ(checkArguments(2, 0, 20), false);
    EXPECT_EQ(checkArguments(2, 7, -120), false);
}

struct ErrorCase {
    const char *arg;
    int expected;
};

class ErrorHandlingHelpTest : public ::testing::TestWithParam<ErrorCase> {};

TEST_P(ErrorHandlingHelpTest, HandleCorrectHelpFlags) {
    const auto &param = GetParam();
    const char *argv[] = { "./plazza",  param.arg};

    EXPECT_EQ(errorHandling(2, argv), param.expected);
}

INSTANTIATE_TEST_SUITE_P(
    HelpFalgs,
    ErrorHandlingHelpTest,
    ::testing::Values(
        ErrorCase{ "-h", 1 },
        ErrorCase{ "--help", 1 },
        ErrorCase{ "--h", -1 },
        ErrorCase{ "-help", -1 },
        ErrorCase{ "help", -1 },
        ErrorCase{ "test", -1 },
        ErrorCase{ "-x", -1 }
    )
);

struct ArgumentsCase {
    const char *arg1;
    const char *arg2;
    const char *arg3;
    int expected;
};

class ErrorHandlingTestArguments : public ::testing::TestWithParam<ArgumentsCase> {};

TEST_P(ErrorHandlingTestArguments, HandleArguments) {
    const auto &param = GetParam();
    const char *argv[] = { "./plazza", param.arg1, param.arg2, param.arg3 };

    std::cout << "Arg1: " << param.arg1 << std::endl;
    std::cout << "Arg2: " << param.arg2 << std::endl;
    std::cout << "Arg3: " << param.arg3 << std::endl;
    EXPECT_EQ(errorHandling(4, argv), param.expected);
}

INSTANTIATE_TEST_SUITE_P(
    Arguments,
    ErrorHandlingTestArguments,
    ::testing::Values(
        ArgumentsCase{ "3", "5", "2000", 0 },
        ArgumentsCase{ "0.5", "2", "400", 0 },
        ArgumentsCase{ "1", "1", "1", 0 },
        ArgumentsCase{ "-h", "5", "2000", -1 },
        ArgumentsCase{ "--help", "5", "2000", -1 },
        ArgumentsCase{ "test", "5", "2000", -1 },
        ArgumentsCase{ "3", "test", "2000", -1 },
        ArgumentsCase{ "3", "5", "test", -1 },
        ArgumentsCase{ "3", "5.7", "2000", -1 },
        ArgumentsCase{ "3", "5", "20.02", -1 },
        ArgumentsCase{ "-3", "5", "20", -1 },
        ArgumentsCase{ "3", "-5", "20", -1 },
        ArgumentsCase{ "3", "5", "-20", -1 }
    )
);