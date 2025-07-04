#pragma once
#include <streambuf>
#include <string>
#include "missle_type.hpp"

class Missle {
  public:
    Missle(std::string name, int od, int ud, int des, int st, MissleType type) :
      name(name), overalDistance(od), uncontrolledDistance(ud), destruction(des), stealth(st), type(type) {}
  private:
      std::string name;
      int overalDistance;
      int uncontrolledDistance;
      int destruction;
      int stealth;
      MissleType type;
};
