#include <iostream>
#include "graph.hpp"
#include "input_handler.hpp"
#include "base_city.hpp"
#include "target_city.hpp"
#include "scenario1.hpp"
#include "inventory.hpp"
#include "scenario3_input.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) return 0;
  Graph g;

  int scenario = std::stoi(argv[1]);
  switch (scenario) {
    case 1:
      // printAllCities();
      InputHandler::loadFromFile(g);
      Scenario1(g).solve();
      // s.solve();
      break;

    case 3:
      Inventory in;
      Scenario3Input::loadFromFile(g, in);
      std::cout << "we have :\n";
      std::cout << "A1: " << in.A1 << " A2: " << in.A2 << " A3: " << in.A3
                << " B1: " << in.B1 << " B2: " << in.B2 << " C1: " << in.C1
                << " C2: " << in.C2 << " D1: " << in.D1 << '\n';

      break;
  }
}