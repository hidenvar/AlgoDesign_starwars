#pragma once

#include "scenario.hpp"
#include "graph.hpp"

class Scenario2 : public Scenario
{
public:
  Scenario2(Graph &g);
  void solve() override;

private:
  struct PathInfo
  {
    Graph::VertexDescriptor base;
    Graph::VertexDescriptor target;
    std::vector<std::string> cities;
    int spyCount;
    double distance;
  };

  struct FallbackBase
  {
    Graph::VertexDescriptor base;
    std::string baseName;
    std::unordered_map<std::string, std::pair<int, int>> missileInventory; // <type, <count, damage>>
    std::vector<PathInfo> paths;
  };

private:
  void initialize() override;
  void findPaths();
  void buildBaseToPathsMap();
  void attack();
  int getTotalDamage();

private:
  std::vector<Graph::VertexDescriptor> baseVertices;
  std::vector<Graph::VertexDescriptor> targetVertices;
  std::unordered_map<Graph::VertexDescriptor, std::vector<PathInfo>> baseToPathsMap;
  std::vector<PathInfo> paths;
};