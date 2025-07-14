#include <cmath>
#include <boost/graph/graph_traits.hpp>
#include "../../include/core/graph.hpp"

void Graph::addCity(Graph::CityPtr city) {
    VertexDescriptor newVertex = boost::add_vertex(city, citiesGraph);
    cityNameToVertex[city->getName()] = newVertex;
}

void Graph::connectCities(std::string& a, std::string& b) {
    auto it1 = cityNameToVertex.find(a);
    auto it2 = cityNameToVertex.find(b);

    if (it1 == cityNameToVertex.end() || it2 == cityNameToVertex.end()) {
        throw std::runtime_error("One or both city names not found.");
    }

    VertexDescriptor v1 = it1->second;
    VertexDescriptor v2 = it2->second;

    const auto city1 = citiesGraph[v1];
    const auto city2 = citiesGraph[v2];

    double distance = calculateDistance(*city1, *city2);
    addEdge(v1, v2, distance);
}

void Graph::addEdge(VertexDescriptor v1, VertexDescriptor v2, double distance) {
    auto edge = boost::add_edge(v1, v2, citiesGraph);
    boost::put(boost::edge_weight, citiesGraph, edge.first, distance);
}

double Graph::calculateDistance(const City& a, const City& b) const {
    double x_diff = b.getLongitude() - a.getLongitude();
    double y_diff = b.getLatitude() - a.getLatitude();
    return std::sqrt(x_diff*x_diff + y_diff*y_diff);
}

Graph::GraphType Graph::getCitiesGraph() const { return citiesGraph; }

std::map<std::string, Graph::VertexDescriptor> Graph::getCitiesVertex() const { return cityNameToVertex; }
