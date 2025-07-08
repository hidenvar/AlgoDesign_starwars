#include "../../include/core/city.hpp"

City::City(const std::string& name, const std::string& country, double lat,
           double lon, CityType type, bool sp)
    : cityName(name),
      countryName(country),
      latitude(lat),
      longitude(lon),
      type(type),
      spy(sp) {}

std::string City::getName() const { return cityName; }
std::string City::getCountry() const { return countryName; }
double City::getLatitude() const { return latitude; }
double City::getLongitude() const { return longitude; }
CityType City::getType() const { return type; }
bool City::hasSpy() const { return spy; }
