#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <city.hpp>
#include <map>
#include <string>

class Graph {
 public:
  using GraphType =
      boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, City,
                            boost::property<boost::edge_weight_t, double> >;
  using VertexDescriptor = GraphType::vertex_descriptor;
  using EdgeDescriptor = GraphType::edge_descriptor;

  void addCity(const City& city);

 private:
  GraphType g;
  std::map<std::string, VertexDescriptor> cityNameToVertex;

  double calculateDistance(const City& a, const City& b) const;
  void connectToAllCities(VertexDescriptor newVertex);
  void addEdge(VertexDescriptor v1, VertexDescriptor v2, double distance);
};