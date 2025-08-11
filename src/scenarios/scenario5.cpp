#include <queue>
#include "scenario5.hpp"
#include "missile_factory.hpp"
#include "iostream"


Scenario5::Scenario5(Graph g, Inventory i): Scenario(g){
    inventory = i;
}

void Scenario5::solve(){
  return;
}

void Scenario5::initialize() {
  // uncontrolledDistance is set to max between all missiles
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

void Scenario5::findPaths() {
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

void Scenario5::buildMissilePathMap() {
  missilePathMap.clear();  // Clear existing entries

  // Define all missile types with their identifiers
  static const std::vector<std::pair<std::string, Missile>> missileList = {
      {"A1", MissileFactory::A1}, {"A2", MissileFactory::A2},
      {"A3", MissileFactory::A3}, {"B1", MissileFactory::B1},
      {"B2", MissileFactory::B2}, {"C1", MissileFactory::C1},
      {"C2", MissileFactory::C2}, {"D1", MissileFactory::D1}};

  for (auto& path : paths) {
    // Update spy count for current path
    path.updatePathSpies(*this);

    // Check against each missile type
    for (const auto& missilePair : missileList) {
      const std::string& missileType = missilePair.first;
      const Missile& missile = missilePair.second;

      // Check range and gap
      if (path.distance <= missile.getOveralDistance() &&
          path.max_gap <= missile.getUncontrolledDistance()) {
        // safe/revealed status
        if (path.spyCount < missile.getStealth()) {
          missilePathMap[missileType + " safe"].push_back(path);
        } else {
          missilePathMap[missileType + " revealed"].push_back(path);
        }
      }
    }
  }
}

void Scenario5::logMissilePaths(std::string logType) {
  auto graph = mapInformation.getCitiesGraph();

  if(logType == "verbose"){
  for (const auto& [category, pathList] : missilePathMap) {
    std::cout << "\n===== " << category << " ===== (" << pathList.size()
              << " paths)\n";

    for (const auto& path : pathList) {
      std::string baseName = graph[path.base]->getName();
      std::string targetName = graph[path.target]->getName();

      std::cout << "\nPath: " << baseName << " -> " << targetName
                << " | Distance: " << path.distance
                << " | Max Gap: " << path.max_gap
                << " | Spies: " << path.spyCount << "\n";

      std::cout << "Cities: ";
      for (const auto& city : path.cities) {
        std::cout << city;
        // Check if city has spy
        auto it = mapInformation.getCitiesVertex().find(city);
        if (it != mapInformation.getCitiesVertex().end()) {
          auto vertex = it->second;
          if (graph[vertex]->hasSpy()) {
            std::cout << "(spy) ";
          } else {
            std::cout << " ";
          }
        }
      }
      std::cout << "\n";
    }
  }
}

  std::cout << "\n======= Summary =======\n";
  const std::vector<std::string> missileOrder = {
        "A1 safe", "A1 revealed",
        "A2 safe", "A2 revealed",
        "A3 safe", "A3 revealed",
        "B1 safe", "B1 revealed",
        "B2 safe", "B2 revealed",
        "C1 safe", "C1 revealed",
        "C2 safe", "C2 revealed",
        "D1 safe", "D1 revealed"
    };
  for (const auto& category : missileOrder) {
        auto it = missilePathMap.find(category);
        if (it != missilePathMap.end()) {
            std::cout << category << ": " << it->second.size() << " paths\n";
        } else {
            std::cout << category << ": 0 paths\n";
        }
    }
}