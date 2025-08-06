#pragma once

#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"

// Scenario3 handles a specific attack simulation with fallback logic
class Scenario3 : public Scenario {
 public:
  Scenario3(Graph, Inventory);

  // Initialization and execution
  void initialize();
  void findPaths();
  void solve();
  void attack();

  // Struct representing a missile path
  struct PathInfo {
    Graph::VertexDescriptor base;
    Graph::VertexDescriptor target;
    std::vector<std::string> cities;
    int spyCount;
    double distance;
    double max_gap;
  };

  // Struct representing a base that can fire in fallback phase
  struct FallbackBase {
    Graph::VertexDescriptor baseDesc;
    std::string baseName;
    std::string missileType;
    int inventoryCount;
    int damagePerMissile;
    std::vector<PathInfo> paths;
  };

  // Struct representing a candidate target for fallback attack
  struct Cand {
    int defenseLevel = 0;
    // List of firing options: (fallback base index, count, damage per missile)
    std::vector<std::tuple<int, int, int>> src;
  };

  // Generates a map of missile type to available paths
  std::unordered_map<std::string, std::vector<PathInfo>> getMissilePathMap();

 private:
  Inventory inventory;
  std::vector<PathInfo> paths; 
  std::vector<Graph::VertexDescriptor> baseVertices;
  std::vector<Graph::VertexDescriptor> targetVertices;

  // Maps: e.g., "B1 safe" -> specific paths for that missile type
  std::unordered_map<std::string, std::vector<PathInfo>> missilePathMap;

  // Builds the missilePathMap
  void buildMissilePathMap();

  // Removes all paths related to a given base from missilePathMap
  void removePathsFromAllMissileMaps(Graph::VertexDescriptor baseDesc);

  // Main fallback attack phase handler
  void attackFallbackPhase();

  // --- Fallback Attack Steps ---

  // Step 1: Collect bases with remaining missiles and paths
  std::vector<FallbackBase> collectFallbackBases();

  // Step 2: Check if any missiles are still available to fire
  bool hasMissilesRemaining(const std::vector<FallbackBase>& bases);

  // Step 3: Build list of candidate targets from bases
  std::unordered_map<std::string, Cand>
  gatherTargets(std::vector<FallbackBase>& bases);

  // Step 4: Select the best target and compute total damage bypassing defense
  std::pair<std::string, int> selectBestTarget(
      const std::unordered_map<std::string, Cand>& targets);
  int calculateBypassedDamage(const Cand& cand);

  // Step 5: Execute fire operation on one selected target
  int executeFire(const std::string& target,
                  Cand& cand,
                  std::vector<FallbackBase>& bases);

  // --- Utility Functions ---

  // Returns how many missiles of a type are still in inventory
  int getInventoryCount(const std::string& missileType);

  // Retrieves the list of cities in the path from base to target
  std::vector<std::string> getPathCities(const FallbackBase& fb,
                                         const std::string& target);
};
