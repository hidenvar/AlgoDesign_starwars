#pragma once
#include "graph.hpp"
#include "inventory.hpp"

namespace Scenario7Input
{
    void fillInventory(std::istream &input, Inventory &inventory, std::unordered_map<std::string, std::pair<int, double>>& pr);
    void createCities(std::istream &input, Graph &citiesGraph);
}