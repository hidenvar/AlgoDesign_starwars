#include <string>
#include "../../include/core/missile.hpp"

Missile::Missile(std::string name, int od, int ud, int des, int st, MissileType type) : name(name), overalDistance(od), uncontrolledDistance(ud), destruction(des), stealth(st), type(type) {}

std::string Missile::getName() const { return name; }

int Missile::getOveralDistance() const { return overalDistance; }

int Missile::getUncontrolledDistance() const { return uncontrolledDistance; }

int Missile::getDestruction() const { return destruction; }

int Missile::getStealth() const { return stealth; }

MissileType Missile::getType() const { return type; }
