#pragma once

#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"

class Scenario7 : public Scenario
{
public:
    Scenario7(Graph, Inventory);

private:
    void initialize();
};