#include <cmath>
#include <boost/graph/graph_traits.hpp>
#include "../../include/core/graph.hpp"

void Graph::addCity(const City& city) {
    VertexDescriptor newVertex = boost::add_vertex(city, citiesGraph);
    cityNameToVertex[city.getName()] = newVertex;
    
    if (boost::num_vertices(citiesGraph) > 1) {
        connectToAllCities(newVertex);
    }
}

void Graph::connectToAllCities(VertexDescriptor newVertex) {
    auto [vi, vi_end] = boost::vertices(citiesGraph);
    for (; vi != vi_end; ++vi) {
        if (*vi != newVertex) {
            double distance = calculateDistance(citiesGraph[newVertex], citiesGraph[*vi]);
            addEdge(newVertex, *vi, distance);
        }
    }
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
