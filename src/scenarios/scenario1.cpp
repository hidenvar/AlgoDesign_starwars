#include <queue>
#include<iostream>

#include <boost/graph/adjacency_list.hpp>

#include "scenario1.hpp"
#include"base_city.hpp"

Scenario1::Scenario1(Graph& g) : Scenario(g) {}

// create a graph that connects two cities if their distance is less than uncontrolledDistance
void Scenario1::initialize() {
  int uncontrolledDistance = 500;

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
        Scenario::mapInformation.connectCities(a, b);
      }
    }
  }
}

// performs bfs on the graph to find all paths from base to target
void Scenario1::findPaths() {
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

    paths.clear(); // Clear previous results

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

            // Check if reached target
            bool isTarget = (std::find(targetVertices.begin(), targetVertices.end(), currentVertex) != targetVertices.end());
            if (isTarget) {
                // Convert path to city names and count spies
                PathInfo pathInfo;
                pathInfo.base = base;
                pathInfo.target = currentVertex;
                int spyCount = 0;
                
                for (auto vd : currentPath) {
                    const auto& city = citiesGraph[vd];
                    pathInfo.cities.push_back(city->getName());
                    if (city->hasSpy()) {
                        spyCount++;
                    }
                }
                
                pathInfo.spyCount = spyCount;
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

void Scenario1::buildBaseToPathsMap() {
  baseToPathsMap.clear();
  auto compareSpyCount = [](const PathInfo& a, const PathInfo& b) {return a.spyCount < b.spyCount;};
  for (const auto& path : paths) {
    baseToPathsMap[path.base].push_back(path);
    sort(baseToPathsMap[path.base].begin(), baseToPathsMap[path.base].end(), compareSpyCount);
  }
}

const std::vector<Scenario1::PathInfo>& Scenario1::getPathsFromBase(Graph::VertexDescriptor base) const {
  static const std::vector<PathInfo> empty;  // empty vector for missing keys
  auto it = baseToPathsMap.find(base);
  return (it != baseToPathsMap.end()) ? it->second : empty;
}

const void Scenario1::attack() {
  int totalDamage = 0;

  auto cities = Scenario1::mapInformation.getCitiesGraph();
  for (const auto base : baseVertices) {
    auto cityPtr = cities[base];
    auto baseCityPtr = std::dynamic_pointer_cast<BaseCity>(cityPtr);
    const auto missiles = baseCityPtr->getMissiles();
    auto paths = Scenario1::baseToPathsMap[base];
    if (paths.empty()) {
      std::cout << "base " << baseCityPtr->getName()
                << " cannot attack at all\n";
    } else {
      for (auto missile : missiles) {
        auto stealth = missile.first.getStealth();
        if (paths[0].spyCount < stealth) {
          // found safe path: shoot and damage += missile damge * missile count
          // print path
          std::cout << "shoot " << missile.second << " "
                    << missile.first.getType()
                    << " missiles using this safe path: \n";
          for (auto x : paths[0].cities) std::cout << x << " ";
          std::cout << "\n";
          auto thisDamage = (missile.first.getDestruction() * missile.second);
          std::cout << "damage of this attach: " << thisDamage << "\n";
          totalDamage += thisDamage;
        } else {
          std::cout << "base " << baseCityPtr->getName()
                    << " has no safe path...";
          // todo: there is no safe path, rapid fire on a common target
          // but how?
        }
      }
    }
  }
  std::cout << "\n"
   << "************"
   << "\n"
   << "total damamge: " << totalDamage;
}

void Scenario1::solve() {
    initialize();
    findPaths();
    buildBaseToPathsMap();
    attack();
}

const std::vector<Scenario1::PathInfo>& Scenario1::getPaths() const {return paths;}
const std::vector<Graph::VertexDescriptor> Scenario1::getBaseVertices() const {return baseVertices;}
const std::vector<Graph::VertexDescriptor> Scenario1::getTargetVertices() const {return targetVertices;}