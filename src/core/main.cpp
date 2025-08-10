#include <iostream>

#include "base_city.hpp"
#include "graph.hpp"
#include "input_handler.hpp"
#include "inventory.hpp"
#include "scenario1.hpp"
#include "scenario3.hpp"
#include "scenario3_input.hpp"
#include "scenario5_input.hpp"
#include "target_city.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) return 0;
  Graph g;

  int scenario = std::stoi(argv[1]);
  switch (scenario) {
    case 1: {
      // printAllCities();
      InputHandler::loadFromFile(g);
      Scenario1(g).solve();
      // s.solve();
      break;
    }

    case 3: {
      Inventory in;
      Scenario3Input::loadFromFile(g, in);
      std::cout << "we have :\n";
      std::cout << "A1: " << in.A1 << " A2: " << in.A2 << " A3: " << in.A3
                << " B1: " << in.B1 << " B2: " << in.B2 << " C1: " << in.C1
                << " C2: " << in.C2 << " D1: " << in.D1 << '\n';

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

      auto printPaths = [](const std::string& label,
                           const std::vector<Scenario3::PathInfo>& vec) {
        std::cout << label << " [" << vec.size() << " paths]:\n";
        for (const auto& path : vec) {
          std::cout << "  Path from " << path.cities.front() << " to "
                    << path.cities.back() << " | Distance: " << path.distance
                    << " | Max Gap: " << path.max_gap
                    << " | Spies: " << path.spyCount << "\n";

          std::cout << "    Cities: ";
          for (size_t i = 0; i < path.cities.size(); ++i) {
            std::cout << path.cities[i];
            if (i < path.cities.size() - 1) {
              std::cout << " -> ";
            }
          }
          std::cout << "\n";
        }
        std::cout << std::endl;
      };

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

    case 4: {
      break;
    }

    case 5: {
      Inventory in;
      Scenario5Input::fillInventory(std::cin, in);
      Scenario5Input::createCities(std::cin, g);
      // TODO: solve for night 1 here
      Scenario5Input::updateSpies(std::cin, g);
      // TODO: solve for night 2 here
      Scenario5Input::updateSpies(std::cin, g);
      // TODO: solve for night 3 here
      Scenario5Input::updateSpies(std::cin, g);
      // TODO: solve for night 4 here
      Scenario5Input::updateSpies(std::cin, g);
      // TODO: solve for night 5 here
      break;
    }
  }
}