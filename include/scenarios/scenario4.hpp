#pragma once

#include "scenario.hpp"

class Scenario4 : public Scenario
{
public:
    Scenario4(Graph& g);
    void initialize() override;
    void solve() override;
private:

};