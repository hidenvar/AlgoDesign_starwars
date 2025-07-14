#pragma once
#include "graph.hpp"

class Scenario {
 public:
  Scenario(Graph& g);
  void printAllCities();

  virtual void initialize(){};
  virtual void solve(){};
  virtual ~Scenario() = default;
  
 private:
  Graph mapInformation;
  
};