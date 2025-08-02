#pragma once

#include "scenario.hpp"
#include "graph.hpp"

class Scenario4 : public Scenario
{
public:
    Scenario4(Graph &g);
    void initialize() override;
    void solve() override;

private:
    struct PathInfo
    {
        Graph::VertexDescriptor base;
        Graph::VertexDescriptor target;
        std::vector<std::string> cities;
        int spyCount;
        double totalDistance;
        double maxGap;
    };

    std::vector<Graph::VertexDescriptor> baseVertices;
    std::vector<Graph::VertexDescriptor> targetVertices;

};