#include <gtest/gtest.h>
#include "Reception.hpp"

TEST(parseOrder, HandleValidOrders) {
    Reception reception(1, 1, 1);

    EXPECT_EQ(reception.parseOrder("Regina XS x1"), 1);
    EXPECT_EQ(reception.getNewOrders().size(), 0);

    EXPECT_EQ(reception.parseOrder("Regina S x1"), 0);
    EXPECT_EQ(reception.getNewOrders().size(), 1);

    EXPECT_EQ(reception.parseOrder("Americana M x7"), 0);
    EXPECT_EQ(reception.getNewOrders().size(), 2);
}

TEST(parseOrder, HandleInvalidOrders) {
    Reception reception(1, 1, 1);

    EXPECT_EQ(reception.parseOrder("Fantasia S x1 7"), 1);

    EXPECT_EQ(reception.parseOrder("Regina S"), 1);
    EXPECT_EQ(reception.parseOrder("Regina S      "), 1);
    EXPECT_EQ(reception.parseOrder("Regina"), 1);
    EXPECT_EQ(reception.parseOrder("     Regina"), 1);
    
    EXPECT_EQ(reception.parseOrder(""), 1);
    EXPECT_EQ(reception.parseOrder("     "), 1);
    
    EXPECT_EQ(reception.parseOrder("Regina Fantasia S x1"), 1);

    EXPECT_EQ(reception.getNewOrders().size(), 0);
}

TEST(parseInput, HandleCorrectInput) {
    Reception reception(1, 1, 1);

    EXPECT_EQ(reception.parseInput("Regina S x1"), 0);
    EXPECT_EQ(reception.getNewOrders().size(), 1);
    
    EXPECT_EQ(reception.parseInput("Regina S x1; Fantasia XL x3"), 0);
    EXPECT_EQ(reception.getNewOrders().size(), 3);
    
    EXPECT_EQ(reception.parseInput("Regina S x1; Fantasia XL x3      ;     Americana XXL x7"), 0);
    EXPECT_EQ(reception.getNewOrders().size(), 6);
}

TEST(parseInput, HandleIncorrectInput) {
    Reception reception(1, 1, 1);

    EXPECT_EQ(reception.parseInput("Regina S x1  g"), 1);
    
    EXPECT_EQ(reception.parseInput("Regina S x1; error Fantasia XL x3"), 1);
    
    EXPECT_EQ(reception.parseInput("Regina S x1 , Fantasia XL x3 ; Americana XXL x7"), 1);

    EXPECT_EQ(reception.getNewOrders().size(), 0);
}

TEST(getInput, HandleInputsInput) {
    Reception reception(1, 1, 1);

    std::istringstream fakeInput("exit\n");
    std::streambuf* originalCin = std::cin.rdbuf(fakeInput.rdbuf());
    EXPECT_EQ(reception.getInput(), 1);

    std::istringstream fakeInput2("status\n");
    std::streambuf* originalCin2 = std::cin.rdbuf(fakeInput2.rdbuf());
    EXPECT_EQ(reception.getInput(), 0);
    
    std::istringstream fakeInput3("Regina XL x3\n");
    std::streambuf* originalCin3 = std::cin.rdbuf(fakeInput3.rdbuf());
    EXPECT_EQ(reception.getInput(), 0);
    
    std::istringstream fakeInput4("Regina XL x3 ; Americana S x1\n");
    std::streambuf* originalCin4 = std::cin.rdbuf(fakeInput4.rdbuf());
    EXPECT_EQ(reception.getInput(), 0);

    std::istringstream fakeInput5("Regina XL x3 ; Americana S 1\n");
    std::streambuf* originalCin5 = std::cin.rdbuf(fakeInput5.rdbuf());
    EXPECT_EQ(reception.getInput(), 0);
}