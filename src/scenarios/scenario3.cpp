#include "scenario3.hpp"

#include <queue>
#include<iostream>
#include "missile_factory.hpp"
#include "base_city.hpp"

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
}


void Scenario3::removePathsFromAllMissileMaps(Graph::VertexDescriptor baseDesc) {

  // clear the ref map
  std::vector<std::string> keys = {"B2 safe", "B2 revealed", "C1 safe", "C1 revealed",
                                   "B1 safe", "B1 revealed", "C2 safe", "C2 revealed"};

  for (const auto& key : keys) {
    auto it = missilePathMap.find(key);
    if (it != missilePathMap.end()) {
      it->second.erase(
        std::remove_if(it->second.begin(), it->second.end(),
                       [&](const PathInfo& p) { return p.base == baseDesc; }),
        it->second.end()
      );
    }
  }

  // clear from main paths vector
  paths.erase(
          std::remove_if(paths.begin(), paths.end(), [&](const PathInfo& p) {
            return p.base == baseDesc;
          }),
          paths.end()
        );
}