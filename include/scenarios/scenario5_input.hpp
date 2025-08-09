#pragma once
#include "graph.hpp"
#include "inventory.hpp"


namespace Scenario5Input {
    void fillInventory(std::istream& input, Inventory& inventory);
    void createCities(std::istream& input, Graph& citiesGraph);
}