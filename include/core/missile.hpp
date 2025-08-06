#pragma once
#include <string>
#include "missile_type.hpp"

class Missile {
  public:
    Missile(std::string name, int od, int ud, int des, int st, MissileType type);
    std::string getName() const;
    int getOveralDistance() const;
    int getUncontrolledDistance() const;
    int getDestruction() const;
    int getStealth() const;
    MissileType getType() const;
    std::string getTypeString() const;
    
  private:
      std::string name;
      int overalDistance;
      int uncontrolledDistance;
      int destruction;
      int stealth;
      MissileType type;
};
