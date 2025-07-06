#pragma once

#include "graph.hpp"

class InputHandler {
public:
    static void makeGraph(std::istream& input, Graph& citiesGraph);
    static void loadFromFile(Graph& citiesGraph);
    
};
