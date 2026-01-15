#include <gtest/gtest.h>
#include "utils/utils.hpp"

TEST(isPositive, HandlesPositives) {
    EXPECT_EQ(isPositive(1), true);
    EXPECT_EQ(isPositive(0), true);
    EXPECT_EQ(isPositive(9876543), true);
}

TEST(isPositive, HandlesNegative) {
    EXPECT_EQ(isPositive(-1), false);
    EXPECT_EQ(isPositive(-876527), false);
}

