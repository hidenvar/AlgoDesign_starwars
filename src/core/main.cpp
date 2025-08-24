#include <iostream>
#include <iomanip>

#include "input_handler.hpp"
#include "scenario3_input.hpp"
#include "scenario5_input.hpp"
#include "scenario7_input.hpp"

#include "graph.hpp"
#include "base_city.hpp"
#include "target_city.hpp"
#include "inventory.hpp"

#include "scenario1.hpp"
#include "scenario2.hpp"
#include "scenario3.hpp"
#include "scenario4.hpp"
#include "scenario5.hpp"
#include "scenario6.hpp"
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
    InputHandler::loadFromFile(g);
    Scenario1(g).solve();
    break;
  }

  case 2:
  {
    InputHandler::loadFromFile(g);
    Scenario2(g).solve();
    break;
  }

  case 3:
  {
    Inventory in;
    Scenario3Input::loadFromFile(g, in);

    Scenario3(g, in).solve();

    break;
  }

  case 4:
  {
    InputHandler::loadFromFile(g);
    Scenario4(g).solve();
    break;
  }

  case 5:
  {
    Scenario5().solve();
    break;
  }
  case 7:
    Scenario7().solve();
    break;
  }
}