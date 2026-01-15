#include "utils/utils.hpp"

bool isFloat(const std::string& str)
{
    if (str.find('.') == std::string::npos)
        return false;

    std::istringstream iss(str);
    float f;

    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}

bool isPositive(int nb) {
    return nb >= 0 ? true : false;
}