#pragma once
#include "city.hpp"

class TargetCity : public City {
  public:
    TargetCity(const std::string& name, const std::string& country, double lat,
       double lon, CityType type, int sp);
    int getDefenseLevel() const;    

  private:
    int defenseLevel;
};
