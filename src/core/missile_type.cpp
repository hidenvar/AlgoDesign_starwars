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

std::ostream& operator<<(std::ostream& os, const MissileType& type) {
    const char* str = nullptr;
    
    switch (type) {
        case MissileType::A1: str = "A1"; break;
        case MissileType::A2: str = "A2"; break;
        case MissileType::A3: str = "A3"; break;
        case MissileType::B1: str = "B1"; break;
        case MissileType::B2: str = "B2"; break;
        case MissileType::C1: str = "C1"; break;
        case MissileType::C2: str = "C2"; break;
        case MissileType::D1: str = "D1"; break;
        default:
            os.setstate(std::ios::failbit);
            return os;
    }
    
    return os << str;
}

MissileType getMissileType(const std::string& str) {
    if      (str == "A1") return MissileType::A1;
    else if (str == "A2") return MissileType::A2;
    else if (str == "A3") return MissileType::A3;
    else if (str == "B1") return MissileType::B1;
    else if (str == "B2") return MissileType::B2;
    else if (str == "C1") return MissileType::C1;
    else if (str == "C2") return MissileType::C2;
    else if (str == "D1") return MissileType::D1;
    else throw std::invalid_argument("Invalid missile type: " + str);
}