#pragma once
#include "graph.hpp"

class Scenario {
 public:
  Scenario(Graph& g);
  void printAllCities();

  virtual void initialize() = 0;
  virtual void solve() = 0;

  virtual ~Scenario() = default;
  
 protected:
  Graph mapInformation;

};