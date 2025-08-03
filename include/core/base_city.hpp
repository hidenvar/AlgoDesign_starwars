#pragma once

#include <vector>

#include "city.hpp"
#include "missile.hpp"

class BaseCity : public City {
  public:
    BaseCity(const std::string& name, const std::string& country, double lat, 
             double lon, CityType type, bool sp, std::vector<std::pair<Missile, int>> mc);
    
    std::vector<std::pair<Missile, int>> getMissiles() const;
    void setCapacity(int cap);
    int getCapacity();
  private:
    std::vector<std::pair<Missile, int>> missileInfo;
    int capacity;
};
