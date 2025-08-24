#pragma once
#include <string>

#include "city_type.hpp"

class City {
 public:
  City() = default;
  
  City(const std::string& name, const std::string& country, double lat,
       double lon, CityType type, bool sp);
  virtual ~City() = default; 
  std::string getName() const;
  std::string getCountry() const;
  double getLatitude() const;
  double getLongitude() const;
  CityType getType() const;
  bool hasSpy() const;
  void setSpy(int sp);

 private:
  std::string cityName;
  std::string countryName;
  double latitude;
  double longitude;
  CityType type;
  bool spy;
};
