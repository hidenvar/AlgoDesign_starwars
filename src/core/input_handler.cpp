#include <iostream>
#include "../../include/core/input_handler.hpp"
#include "../../include/core/city_type.hpp"

Graph InputHandler::makeGraph() const {
  // uncomment to read from file (remember to also uncommnet the reset)
  //freopen("map.txt", "r", stdin);
  int numCities;
  std::cin >> numCities;

  Graph graph;

  for (int i = 0; i < numCities; i++) {
    std::string cityName;
    std::cin >> cityName;
    std::string countryName;
    std::cin >> countryName;
    int latitude;
    std::cin >> latitude;
    int longitude;
    std::cin >> longitude;
    CityType type;
    std::cin >> type;

    City city(cityName, countryName, latitude, longitude, type);
    graph.addCity(city);
  }

  //freopen("/dev/tty", "r", stdin); // reset to console input 

  return graph;
}
