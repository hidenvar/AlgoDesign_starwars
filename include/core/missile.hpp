#pragma once
#include <string>
#include "missile_type.hpp"

class Missile {
  public:
    Missile(std::string name, int od, int ud, int des, int st, MissileType type) :
      name(name), overalDistance(od), uncontrolledDistance(ud), destruction(des), stealth(st), type(type) {}
  private:
      std::string name;
      int overalDistance;
      int uncontrolledDistance;
      int destruction;
      int stealth;
      MissileType type;
};
