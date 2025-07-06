#pragma once
#include <string>

#include "city_type.hpp"

class City {
 public:
  City() = default;
  
  City(const std::string& name, const std::string& country, double lat,
       double lon, CityType type, int sp);
  std::string getName() const;
  std::string getCountry() const;
  double getLatitude() const;
  double getLongitude() const;
  CityType getType() const;
  int getSpies() const;

 private:
  std::string cityName;
  std::string countryName;
  int latitude;
  int longitude;
  CityType type;
  int spies;
};
