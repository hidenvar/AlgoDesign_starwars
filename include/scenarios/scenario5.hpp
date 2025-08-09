#pragma once

#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"


// Scenario 5 tries to deploy 5 successful attacks with at least 1 missies hitting the target
// we will shoot the weakest missiles using safe paths and if no safe path we use the min missiles we have
// this way we are always prepared for more spies
class Scenario5 : public Scenario {
 public:
 Scenario5(Graph, Inventory);
 
  void initialize();
  void findPaths();

  struct PathInfo {
    Graph::VertexDescriptor base;
    Graph::VertexDescriptor target;
    std::vector<std::string> cities;
    int spyCount;
    double distance;
    double max_gap;
  };


  private:
  Inventory inventory;
  std::vector<PathInfo> paths; 
  std::vector<Graph::VertexDescriptor> baseVertices;
  std::vector<Graph::VertexDescriptor> targetVertices;

};