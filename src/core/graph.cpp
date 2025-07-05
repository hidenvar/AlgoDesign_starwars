#include "graph.hpp"
#include <cmath>
#include <boost/graph/graph_traits.hpp>


void Graph::addCity(const City& city) {
    VertexDescriptor newVertex = boost::add_vertex(city, g);
    cityNameToVertex[city.getName()] = newVertex;
    
    if (boost::num_vertices(g) > 1) {
        connectToAllCities(newVertex);
    }
}

void Graph::connectToAllCities(VertexDescriptor newVertex) {
    auto [vi, vi_end] = boost::vertices(g);
    for (; vi != vi_end; ++vi) {
        if (*vi != newVertex) {
            double distance = calculateDistance(g[newVertex], g[*vi]);
            addEdge(newVertex, *vi, distance);
        }
    }
}

void Graph::addEdge(VertexDescriptor v1, VertexDescriptor v2, double distance) {
    auto edge = boost::add_edge(v1, v2, g);
    boost::put(boost::edge_weight, g, edge.first, distance);
}

double Graph::calculateDistance(const City& a, const City& b) const {
    double x_diff = b.getLongitude() - a.getLongitude();
    double y_diff = b.getLatitude() - a.getLatitude();
    return std::sqrt(x_diff*x_diff + y_diff*y_diff);
}