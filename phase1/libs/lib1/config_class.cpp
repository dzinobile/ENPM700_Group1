#include "config_class.hpp"
#include <iostream>

// Constructor for ConfigClass
ConfigClass::ConfigClass(){
    
    // Loads intrinsic filepath from user input
    std::cout << "Enter intrinsics filepath: ";
    std::cin >> intrinsicsPath;

    // Loads extrinsic filepath from user input
    std::cout << "\nEnter extrinsics filepath: ";
    std::cin >> extrinsicsPath;

    // Loads Max detection distance from user input
    std::cout << "\nEnter D max: ";
    std::cin >> D_max_m;

    // Loads Min safety distance from user input
    std::cout << "\nEnter D close: ";
    std::cin >> D_close_m;

    // Reject if D_max_m smaller than D_close
    if (D_max_m <= D_close_m){
        std::cerr << "Error: D close must be smaller than D max" << std::endl;
    }

    // Reject if input distances <= 0
    if (D_max_m <= 0 || D_close_m <= 0){
        std::cerr << "Error: distance inputs must be greater than zero" << std::endl;
    }
}