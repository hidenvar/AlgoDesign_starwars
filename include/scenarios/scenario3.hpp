#pragma once
#include "scenario.hpp"
#include "graph.hpp"
#include "inventory.hpp"

class Scenario3 : public Scenario {
 public:
  Scenario3();
  
  struct PathInfo {
    Graph::VertexDescriptor base;
    Graph::VertexDescriptor target;
    std::vector<std::string> cities;
    int spyCount;
    int max_gap;
  };

 private:
  Inventory inventory;
};