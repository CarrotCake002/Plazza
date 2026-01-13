#include "gtest/gtest.h"
#include "Pizza.hpp"

TEST(parsePizzaType, HandlePizzaTypes) {
    EXPECT_EQ(Pizza::parsePizzaType("Regina"), PizzaType::Regina);
    EXPECT_EQ(Pizza::parsePizzaType("Margarita"), PizzaType::Margarita);
    EXPECT_EQ(Pizza::parsePizzaType("Americana"), PizzaType::Americana);
    EXPECT_EQ(Pizza::parsePizzaType("Fantasia"), PizzaType::Fantasia);
    EXPECT_EQ(Pizza::parsePizzaType("XXL"), PizzaType::Error);
    EXPECT_EQ(Pizza::parsePizzaType("regina"), PizzaType::Error);
    EXPECT_EQ(Pizza::parsePizzaType("AmeRicAna"), PizzaType::Error);
    EXPECT_EQ(Pizza::parsePizzaType("hello"), PizzaType::Error);
    EXPECT_EQ(Pizza::parsePizzaType("75346"), PizzaType::Error);
    EXPECT_EQ(Pizza::parsePizzaType("-6.7"), PizzaType::Error);
    EXPECT_EQ(Pizza::parsePizzaType("x"), PizzaType::Error);
}

TEST(parsePizzaSize, HandlePizzaSizes) {
    EXPECT_EQ(Pizza::parsePizzaSize("S"), PizzaSize::S);
    EXPECT_EQ(Pizza::parsePizzaSize("M"), PizzaSize::M);
    EXPECT_EQ(Pizza::parsePizzaSize("L"), PizzaSize::L);
    EXPECT_EQ(Pizza::parsePizzaSize("XL"), PizzaSize::XL);
    EXPECT_EQ(Pizza::parsePizzaSize("XXL"), PizzaSize::XXL);
    EXPECT_EQ(Pizza::parsePizzaSize("regina"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parsePizzaSize("s"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parsePizzaSize("MM"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parsePizzaSize("XX"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parsePizzaSize("8"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parsePizzaSize("1.2"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parsePizzaSize("-16"), PizzaSize::Error);
}

TEST(parsePizzaAmount, HandlePizzaAmounts) {
    EXPECT_EQ(Pizza::parsePizzaAmount("x1"), 1);
    EXPECT_EQ(Pizza::parsePizzaAmount("x70"), 70);
    EXPECT_EQ(Pizza::parsePizzaAmount("2"), -1);
    EXPECT_EQ(Pizza::parsePizzaAmount("x"), -1);
    EXPECT_EQ(Pizza::parsePizzaAmount("x7.2"), -1);
    EXPECT_EQ(Pizza::parsePizzaAmount("x-10"), -10);
    EXPECT_EQ(Pizza::parsePizzaAmount("x0"), 0);
    EXPECT_EQ(Pizza::parsePizzaAmount("x-0"), 0);
    EXPECT_EQ(Pizza::parsePizzaAmount("-x10"), -1);
    EXPECT_EQ(Pizza::parsePizzaAmount("7x"), -1);
    EXPECT_EQ(Pizza::parsePizzaAmount("xx10"), -1);
}