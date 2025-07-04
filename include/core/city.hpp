#pragma once
#include <string>
#include "city_type.hpp"

class City {
  public:
  private:
      std::string cityName;
      std::string countryName;
      int latitude;
      int longitude;
      CityType type;
};
