#ifndef USAGE_HPP
#define USAGE_HPP

#include <iostream>

#include "utils/utils.hpp"

// Info
#define HELP "Usage:\n\t./plazza [speed_multi] [cook_nb] [restock_time]\n\nWhere:\n\t[speed_multi]   Pizza cooking time multiplier. It takes any positive value, where vales between 0 and 1 will increase the cooking time.\n\n\t[cook_nb]\tThe number of cooks per kitchen. This will determine the amount of orders every kitchen can accept.\n\n\t[restock_timer] This is the amount of time (in milliseconds) it will take to restock 1 unit of every ingredient in the ingredients stock.\n"

// Error
#define ERROR_INVALID_ARG_NB "Error: Invalid number of arguments"
#define ERROR_INVALID_ARG "Error: Invalid argument"
#define ERROR_INVALID_INTEGER "Error: Cook number and restock timer must be positive integers."
#define ERROR_INVALID_ARG_VALUE "Error: Invalid argument value"

int checkArguments(float speed_multi, int cook_nb, int restock_timer);
int errorHandling(int ac, const char **av);

#endif // USAGE_HPP
