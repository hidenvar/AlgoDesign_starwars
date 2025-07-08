#include <climits>
#include <stdexcept>
#include "missile_factory.hpp"

const Missile MissileFactory::A1 = Missile("Shahab7", 2500, 500, 100, 3, MissileType::A1);
const Missile MissileFactory::A2 = Missile("Hormoz3", 2500, 500, 130, 2, MissileType::A2);
const Missile MissileFactory::A3 = Missile("Sejjid", 2500, 500, 25, 4, MissileType::A3);
const Missile MissileFactory::B1 = Missile("Ghadr313", 5000, 500, 90, 2, MissileType::B1);
const Missile MissileFactory::B2 = Missile("Tasua", 5000, 500, 300, 0, MissileType::B2);
const Missile MissileFactory::C1 = Missile("Saeed1", 3000, 700, 110, 2, MissileType::C1);
const Missile MissileFactory::C2 = Missile("Tondar85", 2900, 900, 10, 1, MissileType::C2);
const Missile MissileFactory::D1 = Missile("SangarShekan", INT_MAX, 500, 1488, 2, MissileType::D1);


const Missile& MissileFactory::getMissile(MissileType type) {
    switch (type) {
        case MissileType::A1: return A1;
        case MissileType::A2: return A2;
        case MissileType::A3: return A3;
        case MissileType::B1: return B1;
        case MissileType::B2: return B2;
        case MissileType::C1: return C1;
        case MissileType::C2: return C2;
        case MissileType::D1: return D1;
        default:
            throw std::invalid_argument("Invalid MissileType.");
    }
}