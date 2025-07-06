#include "../../include/core/base_city.hpp"

BaseCity::BaseCity(const std::string& name, const std::string& country, double lat, 
                   double lon, CityType type, bool sp, std::vector<std::pair<Missile, int>> mc)
        : City(name, country, lat, lon, type, sp), missileCount(mc) {}
    
    
