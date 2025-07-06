#include<iostream>
#include "../../include/core/city_type.hpp"


std::istream& operator>>(std::istream& is, CityType& type) {
    std::string input;
    is >> input;
    
    for (char& c : input) {
        c = toupper(c);
    }
    
    if (input == "BASE") {
        type = CityType::BASE;
    } 
    else if (input == "NORMAL") {
        type = CityType::NORMAL;
    }
    else if (input == "TARGET") {
        type = CityType::TARGET;
    }
    else {
        // Set failbit if input doesn't match
        is.setstate(std::ios::failbit);
    }
    // usage is like this 
    /*

    std::cin >> city;
    if (std::cin.fail()) {
        std::cerr << "Invalid city type!\n";
    }

    */
    
    return is;
}