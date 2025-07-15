#include "scenario1.hpp"
#include <queue>
#include <boost/graph/adjacency_list.hpp>

Scenario1::Scenario1(Graph& g) : Scenario(g) {}

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

void Scenario1::findPaths() {
    const auto& citiesGraph = Scenario::mapInformation.getCitiesGraph();
    std::vector<Graph::VertexDescriptor> baseVertices;
    std::vector<Graph::VertexDescriptor> targetVertices;

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

const std::vector<Scenario1::PathInfo>& Scenario1::getPaths() const {return paths;}