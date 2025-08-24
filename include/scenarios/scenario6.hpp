#pragma once
#include "scenario.hpp"
#include "scenario5.hpp"
#include "scenario5_input.hpp"
#include <iostream>

class Scenario6 : public Scenario {
public:
    Scenario6(Graph& graph, Inventory& inventory);
    void solve() override;
    void initialize() override;

private:
    Graph& g;
    Inventory& in;
};
