#define TEST_FD_SANITY
#include "Reception.hpp"

#include <gtest/gtest.h>

TEST(ForkFD, ChildDoesNotInheritPreviousKitchenPipes)
{
    Reception r(1.0f, 1, 1);

    ASSERT_EQ(r.createNewKitchen(), 0);
    ASSERT_EQ(r.createNewKitchen(), 0);

    r.shutdown(); // wait for children

    SUCCEED(); // if a child exited with 42, shutdown would hang or test would fail
}