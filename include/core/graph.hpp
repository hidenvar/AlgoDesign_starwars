#pragma once

#include <map>
#include <memory>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include "city.hpp"

class Graph {
 public:
  using CityPtr = std::shared_ptr<City>;
  using EdgeWeight = boost::property<boost::edge_weight_t, double>;
  using GraphType = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    CityPtr,
    EdgeWeight>;
  using VertexDescriptor = GraphType::vertex_descriptor;
  using EdgeDescriptor = GraphType::edge_descriptor;

  void addCity(CityPtr city);
  GraphType getCitiesGraph() const;
  GraphType& getCitiesGraphRef();
  std::map<std::string, VertexDescriptor> getCitiesVertex() const;
  void connectCities(std::string& a, std::string& b);
  double calculateDistance(const City& a, const City& b) const;
  void setDistanceBetweenCities(const std::string& a, const std::string& b, double distance);
  void updateSpyCount(const std::string& cityName, int newSpyCount);
  
 private:
  GraphType citiesGraph;
  std::map<std::string, VertexDescriptor> cityNameToVertex;

  void addEdge(VertexDescriptor v1, VertexDescriptor v2, double distance);
};
