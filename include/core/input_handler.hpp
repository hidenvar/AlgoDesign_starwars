#pragma once

#include "graph.hpp"

class InputHandler {
public:
    static void createCities(std::istream& input, Graph& citiesGraph);
    static void loadFromFile(Graph& citiesGraph);
    static void makeGraph(std::istream& input, Graph& citiesGraph);
    
};
