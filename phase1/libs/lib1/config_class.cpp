#include "config_class.hpp"
#include <iostream>

ConfigClass::ConfigClass(){

    std::cout << "Enter intrinsics filepath: ";
    std::cin >> intrinsicsPath;

    std::cout << "\nEnter extrinsics filepath: ";
    std::cin >> extrinsicsPath;

    std::cout << "\nEnter D max: ";
    std::cin >> D_max_m;

    std::cout << "\nEnter D close: ";
    std::cin >> D_close_m;
}