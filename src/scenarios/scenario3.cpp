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


// === Public Entry Point for Fallback Phase ===
void Scenario3::attackFallbackPhase() {
    // Prepare fallback bases
    auto fallbackBases = collectFallbackBases();
    int totalFallbackDamage = 0;

    // Continue firing waves until no missiles or no bypassable targets
    while (hasMissilesRemaining(fallbackBases)) {
        auto targets = gatherTargets(fallbackBases);
        if (targets.empty()) break;

        auto [bestTarget, bestDamage] = selectBestTarget(targets);
        if (bestDamage <= 0) break;

        totalFallbackDamage += executeFire(bestTarget, targets[bestTarget], fallbackBases);
    }

    std::cout << "\n**** Fallback total damage: " << totalFallbackDamage << " ****\n";
}

// === Step 1: Collect bases with inventory and valid paths ===
std::vector<Scenario3::FallbackBase> Scenario3::collectFallbackBases() {
    std::vector<FallbackBase> bases;
    const std::vector<std::string> missileOrder = {"B2", "C1", "B1", "C2"};

    for (const auto& type : missileOrder) {
        int count = getInventoryCount(type);
        if (count <= 0) continue;

        auto missile = MissileFactory::getMissile(getMissileType(type));
        int damage = missile.getDestruction();
        auto allPaths = missilePathMap[type + " revealed"];

        for (auto baseV : baseVertices) {
            std::vector<PathInfo> valid;
            for (auto& p : allPaths) {
                if (p.base == baseV) valid.push_back(p);
            }
            if (valid.empty()) continue;

            auto basePtr = std::dynamic_pointer_cast<BaseCity>(mapInformation.getCitiesGraphRef()[baseV]);
            if (!basePtr) continue;

            bases.push_back({
                baseV,
                basePtr->getName(),
                type,
                count,
                damage,
                std::move(valid)
            });
        }
    }
    return bases;
}

// === Step 2: Check if any missiles remain ===
bool Scenario3::hasMissilesRemaining(const std::vector<FallbackBase>& bases) {
    for (const auto& fb : bases) {
        if (fb.inventoryCount > 0) return true;
    }
    return false;
}

// === Step 3: Build candidate targets map ===
std::unordered_map<std::string, Scenario3::Cand>
Scenario3::gatherTargets(std::vector<FallbackBase>& bases) {
    std::unordered_map<std::string, Cand> targets;
    auto& graph = mapInformation.getCitiesGraphRef();
    auto cities = mapInformation.getCitiesVertex();

    for (size_t i = 0; i < bases.size(); ++i) {
        auto& fb = bases[i];
        if (fb.inventoryCount == 0) continue;

        for (auto& path : fb.paths) {
            auto basePtr = std::dynamic_pointer_cast<BaseCity>(graph[path.base]);
            int cap = basePtr ? basePtr->getCapacity() : 0;
            int possible = std::min(fb.inventoryCount, cap);
            if (possible <= 0) continue;

            std::string tgtName = graph[path.target]->getName();
            auto& cand = targets[tgtName];

            if (cand.defenseLevel == 0) {
                auto it = cities.find(tgtName);
                if (it != cities.end()) {
                    auto tgtPtr = std::dynamic_pointer_cast<TargetCity>(graph[it->second]);
                    cand.defenseLevel = tgtPtr ? tgtPtr->getDefenseLevel() : 0;
                }
            }
            cand.src.emplace_back(i, possible, fb.damagePerMissile);
        }
    }
    return targets;
}

// === Step 4: Select best target by bypassed damage ===
std::pair<std::string,int> Scenario3::selectBestTarget(
    const std::unordered_map<std::string, Cand>& targets) {
    std::string best;
    int bestDamage = 0;
    for (auto& [name, cand] : targets) {
        int bypass = calculateBypassedDamage(cand);
        if (bypass > bestDamage) {
            bestDamage = bypass;
            best = name;
        }
    }
    return {best, bestDamage};
}

// === Step 4b: Calculate damage past defense, blocking high-damage first ===
int Scenario3::calculateBypassedDamage(const Cand& cand) {
    int remDef = cand.defenseLevel;
    int total = 0;
    auto list = cand.src; // copy for sorting
    std::sort(list.begin(), list.end(),
              [](auto& a, auto& b) { return std::get<2>(a) > std::get<2>(b); });

    for (auto& [idx, cnt, dmg] : list) {
        int blocked = std::min(cnt, remDef);
        remDef -= blocked;
        total += (cnt - blocked) * dmg;
    }
    return total;
}

// === Step 5: Fire missiles, log, apply blocking, return damage ===
int Scenario3::executeFire(
    const std::string& target,
    Scenario3::Cand& cand,
    std::vector<FallbackBase>& bases) {

    std::cout << "\n*** Rapid-Fire on '" << target
              << "' (defense=" << cand.defenseLevel << ") ***\n";
    int fired=0, blocked=0, damage=0;
    int remDef = cand.defenseLevel;
    auto& sources = cand.src;
    std::sort(sources.begin(), sources.end(),
              [](auto& a, auto& b) { return std::get<2>(a) > std::get<2>(b); });
    auto& graph = mapInformation.getCitiesGraphRef();

    for (auto& [idx, cnt, dmg] : sources) {
        auto& fb = bases[idx];
        auto basePtr = std::dynamic_pointer_cast<BaseCity>(graph[fb.baseDesc]);
        int cap = basePtr ? basePtr->getCapacity() : 0;
        int shots = std::min({fb.inventoryCount, cap, cnt});

        // Log fire details
        std::cout << "Base '" << fb.baseName << "' fires " << shots
                  << " x '" << fb.missileType << "' [dmg=" << dmg << "] via path:";
        for (auto& step : getPathCities(fb, target)) std::cout << " " << step;
        std::cout << "\n";

        for (int i = 0; i < shots; ++i) {
            if (remDef > 0) {
                blocked++; remDef--; }
            else { damage += dmg; }
            fired++;
        }

        // Update state
        fb.inventoryCount -= shots;
        if (basePtr) basePtr->setCapacity(cap - shots);
    }

    std::cout << "Summary: Fired=" << fired
              << ", Blocked=" << blocked
              << ", Bypassed=" << (fired - blocked)
              << ", DamageDone=" << damage << "\n";
    return damage;
}

// === Utility: get inventory count by type ===
int Scenario3::getInventoryCount(const std::string& type) {
    if (type == "B2") return inventory.B2;
    if (type == "C1") return inventory.C1;
    if (type == "B1") return inventory.B1;
    if (type == "C2") return inventory.C2;
    return 0;
}

// === Utility: retrieve path cities for logging ===
std::vector<std::string> Scenario3::getPathCities(
    const FallbackBase& fb, const std::string& target) {
    auto& graph = mapInformation.getCitiesGraphRef();
    for (auto& p : fb.paths) {
        if (graph[p.target]->getName() == target) return p.cities;
    }
    return {};
}
