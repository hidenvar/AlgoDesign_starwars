#pragma once

#include "scenario.hpp"
#include "graph.hpp"

class Scenario2 : public Scenario {
    public:
        Scenario2(Graph& g);
        void initialize() override;
        void solve() override;
}