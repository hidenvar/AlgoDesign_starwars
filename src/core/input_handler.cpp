#include <iostream>
#include <sstream>
#include "../../include/core/input_handler.hpp"
#include "../../include/core/city_type.hpp"
#include "../../include/core/base_city.hpp"
#include "../../include/core/target_city.hpp"

void InputHandler::loadFromFile(Graph &citiesGraph) {
  if (!freopen("map.txt", "r", stdin)) {
    std::cerr << "Error: could not redirect stdin to file\nHint: the input stream will be standard input stream\n";
    freopen("/dev/tty", "r", stdin);
  }
  makeGraph(std::cin, citiesGraph);
}


void InputHandler::makeGraph(std::istream& input, Graph& citiesGraph) {
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) break;

    std::istringstream iss(line);
    std::string name, country;
    CityType type;
    int lat, lon;
    int sp;

    iss >> name >> country >> lat >> lon >> type >> sp;

    if (type == CityType::NORMAL) {
      auto normalCity = std::make_shared<City>(name, country, lat, lon, type, sp);
      citiesGraph.addCity(normalCity);
    }
    else if (type == CityType::BASE) {
      std::vector<std::pair<Missile, int>> missiles;
      std::string missileName;
      int missileCount;
      while(iss >> missileName >> missileCount) {
        // missiles.emplace_back(missileName, missileCount); // todo : need completed missile implementation
      }
      auto baseCity = std::make_shared<BaseCity>(name, country, lat, lon, type, sp, missiles);
      citiesGraph.addCity(baseCity);
    }
    else if (type == CityType::TARGET) {
      int defenseLevel;
      iss >> defenseLevel;
      auto targetCity = std::make_shared<TargetCity>(name, country, lat, lon, type, sp, defenseLevel);
      citiesGraph.addCity(targetCity);
    }
  }
}

