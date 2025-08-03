#include "scenario3.hpp"

void Scenario3::initialize(){
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
