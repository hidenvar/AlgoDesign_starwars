#include <iostream>
#include <sstream>
#include "scenario3_input.hpp"
#include "city.hpp"
#include "base_city.hpp"
#include "missile.hpp"
#include "target_city.hpp"

void fillInventory(std::istream& input, Inventory& inventory);
void createCities(std::istream& input, Graph& citiesGraph);

void Scenario3Input::loadFromFile(Graph &citiesGraph, Inventory& inventory) {
    
    fillInventory(std::cin, inventory);
    createCities(std::cin, citiesGraph);

}

void fillInventory(std::istream& input, Inventory& inventory) {
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) break;

        std::string type;
        int cnt;
        std::istringstream iss(line);
        iss >> type >> cnt;

        if (type == "A1") inventory.A1 = cnt;
        else if (type == "A2") inventory.A2 = cnt;
        else if (type == "A3") inventory.A3 = cnt;
        else if (type == "B1") inventory.B1 = cnt;
        else if (type == "B2") inventory.B2 = cnt;
        else if (type == "C1") inventory.C1 = cnt;
        else if (type == "C2") inventory.C2 = cnt;
        else if (type == "D1") inventory.D1 = cnt;
    }
}

void createCities(std::istream& input, Graph& citiesGraph) {
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
      int capacity;
      iss >> capacity;
      std::vector<std::pair<Missile, int>> emptymissile;
      emptymissile.clear();
      auto baseCity = std::make_shared<BaseCity>(name, country, lat, lon, type, sp, emptymissile);
      baseCity->setCapacity(capacity);
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