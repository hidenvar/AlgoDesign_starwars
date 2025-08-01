#include <iostream>
#include "graph.hpp"
#include "input_handler.hpp"
#include "base_city.hpp"
#include "target_city.hpp"
#include "scenario1.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) return 0;
    Graph g;
    InputHandler::loadFromFile(g);

    int scenario = std::stoi(argv[1]);
    switch (scenario) {
        case 1:
            printAllCities(g);
            break;
        }
}