#include <iostream>
#include <sstream>
#include "../../include/core/input_handler.hpp"
#include "../../include/core/city_type.hpp"
#include "../../include/core/base_city.hpp"
#include "../../include/core/target_city.hpp"
#include "missile_factory.hpp"

void InputHandler::loadFromFile(Graph &citiesGraph) {
  if (!freopen("map.txt", "r", stdin)) {
    std::cerr << "Error: could not redirect stdin to file\nHint: the input stream will be standard input stream\n";
    freopen("/dev/tty", "r", stdin);
  }
  createCities(std::cin, citiesGraph);
  //makeGraph(std::cin, citiesGraph);
}


void InputHandler::createCities(std::istream& input, Graph& citiesGraph) {
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) break;

    std::istringstream iss(line);
    std::string name, country;
    CityType type;
    double lat, lon;
    int sp;

    iss >> name >> country >> lat >> lon >> type >> sp;

    if (type == CityType::NORMAL) {
      auto normalCity = std::make_shared<City>(name, country, lat, lon, type, sp);
      citiesGraph.addCity(normalCity);
    }
    else if (type == CityType::BASE) {
      std::vector<std::pair<Missile, int>> missiles;
      MissileFactory mf;
      int missileTypesCount;
      iss >> missileTypesCount; // how many differnt types of missles do we have

      for(int i = 0; i < missileTypesCount; i++){
        MissileType thisMissileType;
        iss >> thisMissileType;

        int thisMissileCount;
        iss >> thisMissileCount;

        auto tempMissle = mf.getMissile(thisMissileType);
        missiles.emplace_back(tempMissle, thisMissileCount);
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

/*
  note: this graph maker only works if only instantly called after the createCities read half file info
  careful when using and TODO: change structure if possible
*/ 
void InputHandler::makeGraph(std::istream& input, Graph& citiesGraph) {
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) break;
      std::istringstream iss(line);
      std::string it1, it2;
      iss >> it1 >> it2;
      citiesGraph.connectCities(it1, it2);
  }
}


/*
input format for each city type
NORMAL city : name country lat lon type  spyCount
BASE city : name country lat lon type  spyCount missileTypeCount first_missileTye first_TypeCount seconf_missileTye second_TypeCount
TARGET city : name country lat lon type  spyCount defence_level
... required line break here, cinnected cities below
city1 city2
city2 city5

*/