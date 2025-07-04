#include "graph.hpp"

#include <cmath>

void Graph::addCity(const City& city) {
  int idx = cities.size();

  cities.push_back(city);
  cityNameToIndex[city.getName()] = idx;
  adjList.push_back(std::list<std::pair<int, double>>());

  // automatically connect to all existing cities
  if (idx > 0) {
    connectToAllCities(idx);
  }
}

void Graph::addEdge(const int idx1, const int idx2) {
  const auto distance = calculateDistance(cities[idx1], cities[idx2]);

  adjList[idx1].emplace_back(idx2, distance);
  adjList[idx2].emplace_back(idx1, distance);
}

void Graph::connectToAllCities(int newIndex) {
  for (int i = 0; i < newIndex; i++) {
    // connect the new city to all cities
    addEdge(newIndex, i);
  }
}
double Graph::calculateDistance(const City& a, const City& b) const {
  double x_diff = b.getLongitude() - a.getLongitude();
  double y_diff = b.getLatitude() - a.getLatitude();

  // euclidean distance
  return std::sqrt((x_diff * x_diff) + (y_diff * y_diff));
}