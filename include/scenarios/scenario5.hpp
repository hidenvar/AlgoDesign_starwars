#pragma once
#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"

// Scenario 5 tries to deploy 5 successful attacks with at least 1 missile
// hitting the target. We will shoot the weakest missiles using safe paths and
// if no safe path we use the min missiles we have this way we are always
// prepared for more spies
class Scenario5 : public Scenario
{
public:
  Scenario5();
  Scenario5(Graph g, Inventory in);
  void initialize();
  void findPaths();
  void solve() override;

  struct PathInfo
  {
    Graph::VertexDescriptor base;
    Graph::VertexDescriptor target;
    std::vector<std::string> cities;
    int spyCount;
    double distance;
    double max_gap;

    void updatePathSpies(Scenario5 &scenario)
    {
      auto &graph = scenario.mapInformation.getCitiesGraphRef();
      auto cityVertices = scenario.mapInformation.getCitiesVertex();
      spyCount = 0;
      for (const auto &cityName : cities)
      {
        auto it = cityVertices.find(cityName);
        if (it != cityVertices.end())
        {
          auto vertex = it->second;
          auto cityPtr = graph[vertex];
          spyCount += cityPtr->hasSpy();
        }
      }
    }
  };

  struct FallbackBase
  {
    Graph::VertexDescriptor baseDesc;
    std::string baseName;
    std::string missileType;
    int inventoryCount;
    int damagePerMissile;
    std::vector<PathInfo> paths;
  };

  struct AttackStep
  {
    Graph::VertexDescriptor
        baseVertex;
    std::string baseName;
    std::string missileType;
    int missileCount;
    int damagePerMissile;
    std::vector<std::string> pathCities;
  };

  struct MinimalTarget
  {
    std::string targetName;
    int defenseLevel = 0;
    int totalMissilesNeeded = 0;
    std::vector<AttackStep> attackPlan;
  };

  void buildMissilePathMap();
  // normal vs verbose
  void logMissilePaths(std::string logType = "normal");
  void attack(int night);
  std::vector<bool> nights;

private:
  Inventory inventory;
  std::vector<PathInfo> paths;
  std::vector<Graph::VertexDescriptor> baseVertices;
  std::vector<Graph::VertexDescriptor> targetVertices;
  std::unordered_map<std::string, std::vector<PathInfo>> missilePathMap;

  void removePathsFromAllMissileMaps(Graph::VertexDescriptor baseDesc);

  bool attackFallbackPhase();

  MinimalTarget findMostEfficientTarget();

  std::vector<AttackStep> planMinimalAttack(
      const std::vector<FallbackBase> &bases, const std::string &targetName,
      int missilesNeeded);

  bool executeMinimalAttack(const MinimalTarget &target);

  std::vector<std::string> getPathToTarget(const FallbackBase &fb,
                                           const std::string &targetName);

  void updateInventory(const std::string &missileType, int change);

  std::vector<FallbackBase> collectFallbackBases();
  int getInventoryCount(const std::string &type);
};