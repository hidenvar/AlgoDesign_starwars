#pragma once
#include "missile.hpp"

class MissileFactory {
  public:
    static const Missile& getMissile(MissileType type); 

    const static Missile A1;
    const static Missile A2;
    const static Missile A3;
    const static Missile B1;
    const static Missile B2;
    const static Missile C1;
    const static Missile C2;
    const static Missile D1;

    
};