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
  void solve() override;

  struct PathInfo {
    Graph::VertexDescriptor base;
    Graph::VertexDescriptor target;
    std::vector<std::string> cities;
    int spyCount;
    double distance;
    double max_gap;

    void updatePathSpies(Scenario5& scenario) {
      auto& graph = scenario.mapInformation.getCitiesGraphRef();
      auto cityVertices = scenario.mapInformation.getCitiesVertex();

      spyCount = 0;
      for (const auto& cityName : cities) {
        auto it = cityVertices.find(cityName);
        if (it != cityVertices.end()) {
          auto vertex = it->second;
          auto cityPtr = graph[vertex];
          spyCount += cityPtr->hasSpy();
        }
      }
    }
  };

  void buildMissilePathMap();

  private:
  Inventory inventory;
  std::vector<PathInfo> paths; 
  std::vector<Graph::VertexDescriptor> baseVertices;
  std::vector<Graph::VertexDescriptor> targetVertices;
  std::unordered_map<std::string, std::vector<PathInfo>> missilePathMap;
};