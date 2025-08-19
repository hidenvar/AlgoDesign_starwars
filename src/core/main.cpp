#include <iostream>

#include "base_city.hpp"
#include "graph.hpp"
#include "input_handler.hpp"
#include "inventory.hpp"
#include "scenario1.hpp"
#include "scenario3.hpp"
#include "scenario3_input.hpp"
#include "scenario5_input.hpp"
#include "scenario5.hpp"
#include "iomanip"
#include "target_city.hpp"
#include "scenario7_input.hpp"
#include "scenario7.hpp"

auto printPaths = [](const std::string &label,
                     const std::vector<Scenario3::PathInfo> &vec)
{
  std::cout << label << " [" << vec.size() << " paths]:\n";
  for (const auto &path : vec)
  {
    std::cout << "  Path from " << path.cities.front() << " to "
              << path.cities.back() << " | Distance: " << path.distance
              << " | Max Gap: " << path.max_gap
              << " | Spies: " << path.spyCount << "\n";

    std::cout << "    Cities: ";
    for (size_t i = 0; i < path.cities.size(); ++i)
    {
      std::cout << path.cities[i];
      if (i < path.cities.size() - 1)
      {
        std::cout << " -> ";
      }
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
};

void logInventory(Inventory in)
{
  std::cout << "inventory: \n";
  std::cout << "A1: " << in.A1 << " A2: " << in.A2 << " A3: " << in.A3
            << " B1: " << in.B1 << " B2: " << in.B2 << " C1: " << in.C1
            << " C2: " << in.C2 << " D1: " << in.D1 << '\n';
}

void printCities(const Graph &scenarioGraph)
{
  auto graph = scenarioGraph.getCitiesGraph();

  std::cout << std::left
            << std::setw(12) << "Name"
            << std::setw(12) << "Country"
            << std::setw(10) << "Lat"
            << std::setw(10) << "Lon"
            << std::setw(10) << "Type"
            << std::setw(6) << "SP"
            << std::setw(14) << "Extra Info"
            << "\n";

  std::cout << std::string(74, '-') << "\n";

  for (auto vd : boost::make_iterator_range(boost::vertices(graph)))
  {
    auto cityPtr = graph[vd];

    if (!cityPtr)
      continue;

    std::string typeStr;
    switch (cityPtr->getType())
    {
    case CityType::NORMAL:
      typeStr = "NORMAL";
      break;
    case CityType::BASE:
      typeStr = "BASE";
      break;
    case CityType::TARGET:
      typeStr = "TARGET";
      break;
    default:
      typeStr = "UNKNOWN";
      break;
    }

    std::string extraInfo = "-";

    if (auto base = std::dynamic_pointer_cast<BaseCity>(cityPtr))
    {
      extraInfo = "Missiles: " + std::to_string(base->getMissiles().size());
    }
    else if (auto target = std::dynamic_pointer_cast<TargetCity>(cityPtr))
    {
      extraInfo = "Defense: " + std::to_string(target->getDefenseLevel());
    }

    std::cout << std::setw(12) << cityPtr->getName()
              << std::setw(12) << cityPtr->getCountry()
              << std::setw(10) << cityPtr->getLatitude()
              << std::setw(10) << cityPtr->getLongitude()
              << std::setw(10) << typeStr
              << std::setw(6) << cityPtr->hasSpy()
              << std::setw(14) << extraInfo
              << "\n";
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2)
    return 0;
  Graph g;

  int scenario = std::stoi(argv[1]);
  switch (scenario)
  {
  case 1:
  {
    // printAllCities();
    InputHandler::loadFromFile(g);
    Scenario1(g).solve();
    // s.solve();
    break;
  }

  case 3:
  {
    Inventory in;
    Scenario3Input::loadFromFile(g, in);

    Scenario3 s(g, in);
    s.solve();
    auto refmap = s.getMissilePathMap();
    auto vecB1s = refmap["B1 safe"];
    auto vecB1r = refmap["B1 revealed"];
    auto vecB2s = refmap["B2 safe"];
    auto vecB2r = refmap["B2 revealed"];
    auto vecC1s = refmap["C1 safe"];
    auto vecC1r = refmap["C1 revealed"];
    auto vecC2s = refmap["C2 safe"];
    auto vecC2r = refmap["C2 revealed"];

    printPaths("B1 safe", vecB1s);
    printPaths("B1 revealed", vecB1r);
    printPaths("B2 safe", vecB2s);
    printPaths("B2 revealed", vecB2r);
    printPaths("C1 safe", vecC1s);
    printPaths("C1 revealed", vecC1r);
    printPaths("C2 safe", vecC2s);
    printPaths("C2 revealed", vecC2r);

    break;
  }

  case 4:
  {
    break;
  }

  case 5:
  {
    Inventory in;
    Scenario5Input::fillInventory(std::cin, in);
    logInventory(in);
    Scenario5Input::createCities(std::cin, g);
    Scenario5 s(g, in);
    s.initialize();
    s.findPaths();
    // printCities(g);
    s.buildMissilePathMap();
    s.logMissilePaths();
    // TODO: solve for night 1 here
    s.attack(0);
    Scenario5Input::updateSpies(std::cin, g);
    // printCities(g);
    s.buildMissilePathMap();
    s.logMissilePaths();
    // TODO: solve for night 2 here
    s.attack(1);
    Scenario5Input::updateSpies(std::cin, g);
    // printCities(g);
    s.buildMissilePathMap();
    s.logMissilePaths();
    // TODO: solve for night 3 here
    s.attack(2);
    Scenario5Input::updateSpies(std::cin, g);
    // printCities(g);
    s.buildMissilePathMap();
    s.logMissilePaths();
    // TODO: solve for night 4 here
    s.attack(3);
    Scenario5Input::updateSpies(std::cin, g);
    // printCities(g);
    s.buildMissilePathMap();
    s.logMissilePaths();
    // TODO: solve for night 5 here
    s.attack(4);

    for (auto n : s.nights)
      std::cout << (n ? "✅" : "❌") << " ";
    break;
  }
  case 7:
    std::unordered_map<std::string, std::pair<int, double>> pr;
    Inventory in;
    int ds;
    Scenario7Input::fillInventory(std::cin, in, pr);
    Scenario7Input::createCities(std::cin, g);
    Scenario7Input::setDesiredDamage(std::cin, ds);

    Scenario7 s(g, in, pr, ds);
    s.initialize();
    s.findPaths();
    s.buildMissilePathMap();
    std::map<std::string, int> usedMissiles;
    int cost = s.findMinimumCost(usedMissiles);
    if (cost == -1)
    {
      std::cout << "Impossible to reach damage  with available missiles.\n";
    }
    else
    {
      std::cout << "Minimum cost: " << cost << "\nMissiles used:\n";
      for (const auto &[name, count] : usedMissiles)
      {
        std::cout << "  " << name << ": " << count << "\n";
      }
    }
    // logInventory(in);
  }
}