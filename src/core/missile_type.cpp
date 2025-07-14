#include <iostream>
#include <stdexcept>
#include "missile_type.hpp"

std::istream& operator>>(std::istream& is, MissileType& type) {
    std::string str;
    if (is >> str) {
        if      (str == "A1") type = MissileType::A1;
        else if (str == "A2") type = MissileType::A2;
        else if (str == "A3") type = MissileType::A3;
        else if (str == "B1") type = MissileType::B1;
        else if (str == "B2") type = MissileType::B2;
        else if (str == "C1") type = MissileType::C1;
        else if (str == "C2") type = MissileType::C2;
        else if (str == "D1") type = MissileType::D1;
        else {
            is.setstate(std::ios::failbit);
        }
    }
    return is;
}