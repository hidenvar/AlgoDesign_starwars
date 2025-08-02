#include <iostream>
#include <sstream>
#include "scenario3_input.hpp"

void fillInventory(std::istream& input, Inventory& inventory);
void Scenario3Input::loadFromFile(Graph &citiesGraph, Inventory& inventory) {
    
    fillInventory(std::cin, inventory);
    // create cities graph

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