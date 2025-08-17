#pragma once

#include "graph.hpp"
#include "inventory.hpp"
#include "scenario.hpp"

class Scenario7 : public Scenario
{
public:
    Scenario7(Graph, Inventory);

private:
    void initialize();
    void findPaths();

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
    std::vector<Graph::VertexDescriptor> baseVertices;
    std::vector<Graph::VertexDescriptor> targetVertices;
    std::vector<PathInfo> paths;
};