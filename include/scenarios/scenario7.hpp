#pragma once

#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"

class Scenario7 : public Scenario
{
public:
    Scenario7(Graph, Inventory, std::unordered_map<std::string, std::pair<int, double>>);

public:
    void initialize();
    void findPaths();
    void buildMissilePathMap();
    void solve() override;

private:
    struct PathInfo
    {
        Graph::VertexDescriptor base;
        Graph::VertexDescriptor target;
        std::vector<std::string> cities;
        int spyCount;
        double distance;
        double max_gap;
    };

private:
    Inventory inventory;
    std::unordered_map<std::string, std::pair<int, double>> pr;
    std::unordered_map<std::string, std::vector<PathInfo>> missilePathMap;
    std::vector<Graph::VertexDescriptor> baseVertices;
    std::vector<Graph::VertexDescriptor> targetVertices;
    std::vector<PathInfo> paths;
};