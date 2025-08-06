#pragma once

#include "scenario.hpp"
#include "graph.hpp"

class Scenario4 : public Scenario
{
public:
    Scenario4(Graph &g);
    void solve() override;


private:
    void findPaths();
    void buildPaths();
    void printPathInfo() const;
    void initialize() override;
    void attack();

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
    std::vector<PathInfo> paths;
    std::unordered_map<std::string, std::vector<PathInfo>> missilePathMap; // map each missile to a revealed or a safe path
};