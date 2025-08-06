#include <string>
#include "../../include/core/missile.hpp"

Missile::Missile(std::string name, int od, int ud, int des, int st, MissileType type) : name(name), overalDistance(od), uncontrolledDistance(ud), destruction(des), stealth(st), type(type) {}

std::string Missile::getName() const { return name; }

int Missile::getOveralDistance() const { return overalDistance; }

int Missile::getUncontrolledDistance() const { return uncontrolledDistance; }

int Missile::getDestruction() const { return destruction; }

int Missile::getStealth() const { return stealth; }

MissileType Missile::getType() const { return type; }

std::string Missile::getTypeString() const {
    switch (type) {
        case MissileType::A1: return "A1";
        case MissileType::A2: return "A2";
        case MissileType::A3: return "A3";
        case MissileType::B1: return "B1";
        case MissileType::B2: return "B2";
        case MissileType::C1: return "C1";
        case MissileType::C2: return "C2";
        case MissileType::D1: return "D1";
        default: return "Unknown";
    }
}