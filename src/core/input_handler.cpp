#include <iostream>
#include <sstream>
#include "../../include/core/input_handler.hpp"
#include "../../include/core/city_type.hpp"

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
    double lat, lon;
    int sp;

    iss >> name >> country >> lat >> lon >> type >> sp;
    
  }

}

