#pragma once
#include <string>
#include "missile_type.hpp"

class Missile {
  public:
  private:
      std::string name;
      int overalDistance;
      int uncontrolledDistance;
      int destruction;
      int stealth;
      MissileType type;
};
