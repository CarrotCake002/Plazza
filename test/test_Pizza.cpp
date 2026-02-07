#include "gtest/gtest.h"
#include "Pizza.hpp"

TEST(parseType, HandlePizzaTypes) {
    EXPECT_EQ(Pizza::parseType("Regina"), PizzaType::Regina);
    EXPECT_EQ(Pizza::parseType("Margarita"), PizzaType::Margarita);
    EXPECT_EQ(Pizza::parseType("Americana"), PizzaType::Americana);
    EXPECT_EQ(Pizza::parseType("Fantasia"), PizzaType::Fantasia);
    EXPECT_EQ(Pizza::parseType("XXL"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("regina"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("AmeRicAna"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("hello"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("75346"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("-6.7"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("x"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("Reginabgsd"), PizzaType::Error);
    EXPECT_EQ(Pizza::parseType("gfdsgbRegina"), PizzaType::Error);
}

TEST(parseSize, HandlePizzaSizes) {
    EXPECT_EQ(Pizza::parseSize("S"), PizzaSize::S);
    EXPECT_EQ(Pizza::parseSize("M"), PizzaSize::M);
    EXPECT_EQ(Pizza::parseSize("L"), PizzaSize::L);
    EXPECT_EQ(Pizza::parseSize("XL"), PizzaSize::XL);
    EXPECT_EQ(Pizza::parseSize("XXL"), PizzaSize::XXL);
    EXPECT_EQ(Pizza::parseSize("regina"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parseSize("s"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parseSize("MM"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parseSize("XX"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parseSize("8"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parseSize("1.2"), PizzaSize::Error);
    EXPECT_EQ(Pizza::parseSize("-16"), PizzaSize::Error);
}

TEST(parseAmount, HandlePizzaAmounts) {
    EXPECT_EQ(Pizza::parseAmount("x1"), 1);
    EXPECT_EQ(Pizza::parseAmount("x70"), 70);
    EXPECT_EQ(Pizza::parseAmount("2"), -1);
    EXPECT_EQ(Pizza::parseAmount("x"), -1);
    EXPECT_EQ(Pizza::parseAmount("x7.2"), -1);
    EXPECT_EQ(Pizza::parseAmount("x-10"), -10);
    EXPECT_EQ(Pizza::parseAmount("x0"), 0);
    EXPECT_EQ(Pizza::parseAmount("x-0"), 0);
    EXPECT_EQ(Pizza::parseAmount("-x10"), -1);
    EXPECT_EQ(Pizza::parseAmount("7x"), -1);
    EXPECT_EQ(Pizza::parseAmount("xx10"), -1);
}

TEST(typeToString, HandleTypeConversion) {
    EXPECT_EQ(Pizza::typeToString(PizzaType::Regina), "Regina");
    EXPECT_EQ(Pizza::typeToString(PizzaType::Americana), "Americana");
    EXPECT_EQ(Pizza::typeToString(PizzaType::Fantasia), "Fantasia");
    EXPECT_EQ(Pizza::typeToString(PizzaType::Margarita), "Margarita");
    EXPECT_EQ(Pizza::typeToString(PizzaType::Error), "Error"); 
}

TEST(sizeToString, HandleSizeConversion) {
    EXPECT_EQ(Pizza::sizeToString(PizzaSize::S), "S");
    EXPECT_EQ(Pizza::sizeToString(PizzaSize::M), "M");
    EXPECT_EQ(Pizza::sizeToString(PizzaSize::L), "L");
    EXPECT_EQ(Pizza::sizeToString(PizzaSize::XL), "XL");
    EXPECT_EQ(Pizza::sizeToString(PizzaSize::XXL), "XXL");
    EXPECT_EQ(Pizza::sizeToString(PizzaSize::Error), "Error");
}

TEST(amountToString, HandleAmountConversion) {
    EXPECT_EQ(Pizza::amountToString(1), "x1");
    EXPECT_EQ(Pizza::amountToString(85446), "x85446");
    EXPECT_EQ(Pizza::amountToString(-1), "x0");
    EXPECT_EQ(Pizza::amountToString(0), "x0");
    EXPECT_EQ(Pizza::amountToString(-866446), "x0");
}