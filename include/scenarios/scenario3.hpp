#pragma once
#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"

class Scenario3 : public Scenario {
 public:
  Scenario3();
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

  // map : "B1 safe" -> vector of specific paths. 
  std::unordered_map<std::string, std::vector<PathInfo>> missilePathMap;
  void buildMissilePathMap();

};