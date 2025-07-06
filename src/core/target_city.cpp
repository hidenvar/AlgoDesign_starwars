#include "../../include/core/target_city.hpp"

TargetCity::TargetCity(const std::string& name, const std::string& country, double lat,
                       double lon, CityType type, bool sp, int dl)
          : City(name, country, lat, lon, type, sp), defenseLevel(dl) {}
