#pragma once

#include "city.hpp"
#include <vector>
#include <list>
#include <unordered_map>
#include <string>

class Graph {
public:
    void addCity(const City& city);

private:
    double calculateDistance(const City& a, const City& b) const;
    void addEdge(const int idx_city1, const int idx_city2);
    void connectToAllCities(int newIndex);

    std::vector<City> cities;
    std::vector<std::list<std::pair<int, double>>> adjList;
    std::unordered_map<std::string, int> cityNameToIndex;
};