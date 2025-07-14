#pragma once
#include"scenario.hpp"

class Scenario1 : public Scenario{
    public:
    Scenario1(Graph& g);

    void initialize() override;
    void solve() override {}

};