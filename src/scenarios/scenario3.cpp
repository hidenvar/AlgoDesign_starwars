#include "scenario3.hpp"

#include <queue>
#include<iostream>
#include "missile_factory.hpp"
#include "base_city.hpp"
#include "target_city.hpp"

void Scenario3::solve(){
    initialize();
    findPaths();
    buildMissilePathMap();
    attack();
}
Scenario3::Scenario3(Graph g, Inventory i): Scenario(g){
    inventory = i;
}

void Scenario3::initialize() {
  // uncontrolledDistance is set to max between b1, b2, c1, c2
  int uncontrolledDistance = 900;

  auto citiesGraph = Scenario::mapInformation.getCitiesGraph();
  auto cityVertices = Scenario::mapInformation.getCitiesVertex();

  // convert to vector
  std::vector<std::pair<std::string, Graph::VertexDescriptor>> cities(
      cityVertices.begin(), cityVertices.end());

  // iterate over all cities pairs
  for (size_t i = 0; i < cities.size(); ++i) {
    for (size_t j = i + 1; j < cities.size(); ++j) {
      const std::string& city1Name = cities[i].first;
      const std::string& city2Name = cities[j].first;

      auto city1 = citiesGraph[cities[i].second];
      auto city2 = citiesGraph[cities[j].second];

      // calculate distance
      double distance =
          Scenario::mapInformation.calculateDistance(*city1, *city2);

      // connect  if distance <= uncontrolledDistance
      if (distance <= uncontrolledDistance) {
        std::string a = city1Name;
        std::string b = city2Name;
        // add edge between cities
        Scenario::mapInformation.connectCities(a, b);
        // set edge weight
        Scenario::mapInformation.setDistanceBetweenCities(a, b, distance);
      }
    }
  }
}

void Scenario3::findPaths() {
  const auto& citiesGraph = Scenario::mapInformation.getCitiesGraph();
  // std::vector<Graph::VertexDescriptor> baseVertices;
  // std::vector<Graph::VertexDescriptor> targetVertices;

  // Identify base and target cities
  auto vertices = boost::vertices(citiesGraph);
  for (auto vit = vertices.first; vit != vertices.second; ++vit) {
    const auto& city = citiesGraph[*vit];
    if (city->getType() == CityType::BASE) {
      baseVertices.push_back(*vit);
    } else if (city->getType() == CityType::TARGET) {
      targetVertices.push_back(*vit);
    }
  }

  paths.clear();  // Clear previous results

  // BFS for each base city
  for (auto base : baseVertices) {
    std::queue<std::vector<Graph::VertexDescriptor>> q;
    std::unordered_set<Graph::VertexDescriptor> visited;

    q.push({base});
    visited.insert(base);

    while (!q.empty()) {
      auto currentPath = q.front();
      q.pop();
      auto currentVertex = currentPath.back();

      bool isTarget = (std::find(targetVertices.begin(), targetVertices.end(),
                                 currentVertex) != targetVertices.end());
      if (isTarget) {
        PathInfo pathInfo;
        pathInfo.base = base;
        pathInfo.target = currentVertex;
        int spyCount = 0;
        double maxGap = 0.0;
        double totalDistance = 0.0;

        const auto& weightMap = boost::get(boost::edge_weight, citiesGraph);

        for (size_t i = 0; i < currentPath.size(); ++i) {
          const auto& city = citiesGraph[currentPath[i]];
          pathInfo.cities.push_back(city->getName());
          if (city->hasSpy()) {
            spyCount++;
          }

          if (i > 0) {
            const auto &prevCity = citiesGraph[currentPath[i - 1]];
            totalDistance += Scenario::mapInformation.calculateDistance(*prevCity, *city);

            auto [e, exists] =
                boost::edge(currentPath[i - 1], currentPath[i], citiesGraph);
            if (exists) {
              maxGap = std::max(maxGap, (weightMap[e]));
            }
          }
        }

        pathInfo.distance = totalDistance;
        pathInfo.spyCount = spyCount;
        pathInfo.max_gap = maxGap;
        paths.push_back(pathInfo);
      }

      // Explore neighbors
      auto neighbors = boost::adjacent_vertices(currentVertex, citiesGraph);
      for (auto nit = neighbors.first; nit != neighbors.second; ++nit) {
        auto neighbor = *nit;

        if (visited.find(neighbor) == visited.end()) {
          visited.insert(neighbor);
          auto newPath = currentPath;
          newPath.push_back(neighbor);
          q.push(newPath);
        }
      }
    }
  }
}


void Scenario3::buildMissilePathMap(){
    for (const auto& path : paths) {
        // --- B1: od = 5000, ud = 500, st = 2
        if (path.distance < 5000 && path.max_gap < 500) {
            if (path.spyCount < 2) {
                missilePathMap["B1 safe"].push_back(path);
            } else {
                missilePathMap["B1 revealed"].push_back(path);
            }
        }

        // --- B2: od = 5000, ud = 500, st = 0
        if (path.distance < 5000 && path.max_gap < 500) {
            if (path.spyCount < 0) {
                missilePathMap["B2 safe"].push_back(path);
            } else {
                missilePathMap["B2 revealed"].push_back(path);
            }
        }

        // --- C1: od = 3000, ud = 700, st = 2
        if (path.distance < 3000 && path.max_gap < 700) {
            if (path.spyCount < 2) {
                missilePathMap["C1 safe"].push_back(path);
            } else {
                missilePathMap["C1 revealed"].push_back(path);
            }
        }

        // --- C2: od = 2900, ud = 900, st = 1
        if (path.distance < 2900 && path.max_gap < 900) {
            if (path.spyCount < 1) {
                missilePathMap["C2 safe"].push_back(path);
            } else {
                missilePathMap["C2 revealed"].push_back(path);
            }
        }
    }
}


std::unordered_map<std::string, std::vector<Scenario3::PathInfo>> Scenario3::getMissilePathMap(){return missilePathMap;}

void Scenario3::attack() {
  int totalDamage = 0;
  // missile types are c and b
  // we have limited capacity in each base
  // first we shoot highest damage missile through a safe path
  // missile damages : B1 90 / B2 300 / C1 110 / C2 10
  // missile priority : B2, C1, B1, C2

  std::cout << "**** Starting safe attack phase *****\n";

  auto& graph = mapInformation.getCitiesGraphRef();
  std::vector<std::string> missilePriority = {"B2", "C1", "B1", "C2"};

  for (const auto& mtypeStr : missilePriority) {
    int& count = [&]() -> int& {
      if (mtypeStr == "B2") return inventory.B2;
      if (mtypeStr == "C1") return inventory.C1;
      if (mtypeStr == "B1") return inventory.B1;
      return inventory.C2;
    }();

    if (count <= 0) continue;

    auto mt = getMissileType(mtypeStr);
    auto missile = MissileFactory::getMissile(mt);
    int damagePerMissile = missile.getDestruction();

    auto& missilePaths = missilePathMap[mtypeStr + " safe"];

    for (auto it = missilePaths.begin(); it != missilePaths.end(); /* no ++ */) {
      auto baseDesc = it->base;

      auto baseIt = std::find(baseVertices.begin(), baseVertices.end(), baseDesc);
      if (baseIt == baseVertices.end()) {
        ++it;
        continue;
      }

      auto baseCityPtr = std::dynamic_pointer_cast<BaseCity>(graph[baseDesc]);
      if (!baseCityPtr) {
        ++it;
        continue;
      }

      int baseCap = baseCityPtr->getCapacity();
      if (baseCap == 0) {
        baseVertices.erase(baseIt);

        // full cleanup for this base
        removePathsFromAllMissileMaps(baseDesc);

        // Current iterator is now invalid
        it = missilePaths.begin();  // Restart loop safely
        continue;
      }

      // Fire
      int used = std::min(count, baseCap);
      int pathDamage = damagePerMissile * used;
      totalDamage += pathDamage;
      count -= used;
      baseCityPtr->setCapacity(baseCap - used);

      std::cout << "Shot " << used << "x " << mtypeStr << " missiles via path: ";
      for (const auto& cityName : it->cities) {
        std::cout << cityName << " ";
      }
      std::cout << "| Damage: " << pathDamage << "\n";

      if (baseCityPtr->getCapacity() == 0) {
        baseVertices.erase(baseIt);

        removePathsFromAllMissileMaps(baseDesc);

        paths.erase(
          std::remove_if(paths.begin(), paths.end(), [&](const PathInfo& p) {
            return p.base == baseDesc;
          }),
          paths.end()
        );

        it = missilePaths.begin();  // Start over
        continue;
      }

      if (count == 0) break;

      ++it;
    }
  }

  std::cout << "Total Damage: " << totalDamage << "\n";

  attackFallbackPhase();
}


void Scenario3::attackFallbackPhase() {
  int totalDamage = 0;

  auto& graph = mapInformation.getCitiesGraphRef();
  auto cities = mapInformation.getCitiesVertex();

  struct FallbackBase {
    Graph::VertexDescriptor baseDesc;
    std::string baseName;
    std::string missileType;
    int inventoryCount;
    int damagePerMissile;
    std::vector<PathInfo> paths;
  };

  // Build all base+missile type combinations with valid revealed paths
  std::vector<FallbackBase> fallbackBases;
  const std::vector<std::string> missileTypes = {"B2", "C1", "B1", "C2"};
  for (const auto& mtypeStr : missileTypes) {
    int inv = (mtypeStr == "B2"   ? inventory.B2
               : mtypeStr == "C1" ? inventory.C1
               : mtypeStr == "B1" ? inventory.B1
                                  : inventory.C2);
    if (inv <= 0) continue;

    auto mt = getMissileType(mtypeStr);
    auto missile = MissileFactory::getMissile(mt);
    int dmg = missile.getDestruction();

    auto keyIt = missilePathMap.find(mtypeStr + " revealed");
    if (keyIt == missilePathMap.end()) continue;
    auto& allPaths = keyIt->second;

    for (auto baseDesc : baseVertices) {
      std::vector<PathInfo> ps;
      for (auto& p : allPaths) {
        if (p.base == baseDesc) {
          ps.push_back(p);
        }
      }
      if (ps.empty()) continue;

      auto ptr = std::dynamic_pointer_cast<BaseCity>(graph[baseDesc]);
      if (!ptr) continue;

      fallbackBases.push_back(
          {baseDesc, ptr->getName(), mtypeStr, inv, dmg, std::move(ps)});
    }
  }

  // Rapid-fire loop
  while (true) {
    // Check if any missiles remain
    bool missilesLeft = false;
    for (auto& fb : fallbackBases) {
      if (fb.inventoryCount > 0) {
        missilesLeft = true;
        break;
      }
    }
    if (!missilesLeft) break;

    // Step 1: Accumulate potential bypassed damage per target
    struct Cand {
      int totalDamage = 0;
      std::vector<int> srcIndices;
    };
    std::unordered_map<std::string, Cand> candidates;

    for (int i = 0; i < (int)fallbackBases.size(); ++i) {
      auto& fb = fallbackBases[i];
      if (fb.inventoryCount == 0) continue;

      for (auto& p : fb.paths) {
        auto basePtr = std::dynamic_pointer_cast<BaseCity>(graph[p.base]);
        int baseCap = basePtr ? basePtr->getCapacity() : 0;
        if (baseCap <= 0) continue;

        std::string tgtName = graph[p.target]->getName();
        auto it = cities.find(tgtName);
        if (it == cities.end()) continue;

        auto tgtCity = std::dynamic_pointer_cast<TargetCity>(graph[it->second]);
        if (!tgtCity) continue;

        int defLv = tgtCity->getDefenseLevel();
        int missilesFromThisBase = std::min(fb.inventoryCount, baseCap);
        int bypassedCount = std::max(0, missilesFromThisBase - defLv);
        int bypassedDamage = bypassedCount * fb.damagePerMissile;

        auto& cd = candidates[tgtName];
        cd.totalDamage += bypassedDamage;
        cd.srcIndices.push_back(i);
      }
    }
    if (candidates.empty()) break;

    // Step 2: Choose best target
    std::string bestTarget;
    int bestDefense = 0;
    int bestTotalDamage = -1;
    for (auto& [name, cd] : candidates) {
      if (cd.totalDamage > bestTotalDamage) {
        bestTotalDamage = cd.totalDamage;
        bestTarget = name;
        auto it = cities.find(name);
        if (it != cities.end()) {
          auto tgtCity = std::dynamic_pointer_cast<TargetCity>(graph[it->second]);
          bestDefense = tgtCity ? tgtCity->getDefenseLevel() : 0;
        }
      }
    }
    if (bestTotalDamage <= 0) break;

    std::cout << "\n*** Rapid-Fire on '" << bestTarget
              << "' (defense=" << bestDefense << ") ***\n";

    // Step 3: Collect all missiles to be fired at best target
    struct MissileStrike {
      int fbIndex;
      int damage;
      Graph::VertexDescriptor baseDesc;
      PathInfo path;
    };
    std::vector<MissileStrike> strikes;

    for (int idx : candidates[bestTarget].srcIndices) {
      auto& fb = fallbackBases[idx];
      if (fb.inventoryCount == 0) continue;

      auto pathIt = std::find_if(
          fb.paths.begin(), fb.paths.end(),
          [&](auto& p) { return graph[p.target]->getName() == bestTarget; });
      if (pathIt == fb.paths.end()) continue;

      auto basePtr = std::dynamic_pointer_cast<BaseCity>(graph[fb.baseDesc]);
      int baseCap = basePtr ? basePtr->getCapacity() : 0;
      int canFire = std::min({fb.inventoryCount, baseCap});
      for (int i = 0; i < canFire; ++i) {
        strikes.push_back({idx, fb.damagePerMissile, fb.baseDesc, *pathIt});
      }
    }

    // Step 4: Sort strikes by damage descending (defense blocks strongest first)
    std::sort(strikes.begin(), strikes.end(), [](const MissileStrike& a, const MissileStrike& b) {
      return a.damage > b.damage;
    });

    // Step 5: Fire missiles
    int fired = 0, blocked = 0;
    for (const auto& s : strikes) {
      auto& fb = fallbackBases[s.fbIndex];
      auto basePtr = std::dynamic_pointer_cast<BaseCity>(graph[s.baseDesc]);
      if (!basePtr) continue;

      if (fb.inventoryCount <= 0 || basePtr->getCapacity() <= 0) continue;

      // Consume inventory and base capacity
      fb.inventoryCount -= 1;
      basePtr->setCapacity(basePtr->getCapacity() - 1);

      // Log missile path
      std::cout << "Base '" << fb.baseName << "' fires 1 x '" << fb.missileType
                << "' via path:";
      for (auto& step : s.path.cities) std::cout << " " << step;
      std::cout << "\n";

      // Apply defense
      if (blocked < bestDefense) {
        blocked++;
      } else {
        totalDamage += fb.damagePerMissile;
      }
      fired++;
    }

    std::cout << "Summary: Fired=" << fired << ", Blocked=" << blocked
              << ", Bypassed=" << (fired - blocked) << "\n";
  }

  std::cout << "\n**** Fallback total damage: " << totalDamage << " ****\n\n\n";
}

void Scenario3::removePathsFromAllMissileMaps(Graph::VertexDescriptor baseDesc) {
    for (auto& [key, paths] : missilePathMap) {
        paths.erase(
            std::remove_if(paths.begin(), paths.end(),
                           [&](const PathInfo& path) {
                               return path.base == baseDesc;
                           }),
            paths.end());
    }
}

