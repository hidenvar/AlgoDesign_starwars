#pragma once
#include <string>
#include "missile_type.hpp"

class Missile {
  public:
<<<<<<< HEAD:include/core/missle.hpp
=======
    Missile(std::string name, int od, int ud, int des, int st, MissileType type) :
      name(name), overalDistance(od), uncontrolledDistance(ud), destruction(des), stealth(st), type(type) {}
>>>>>>> d66fd3cd (chore(missile): fix the wrong spellings):include/core/missile.hpp
  private:
      std::string name;
      int overalDistance;
      int uncontrolledDistance;
      int destruction;
      int stealth;
      MissileType type;
};
