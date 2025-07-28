#pragma once

#include "scenario.hpp"
#include "graph.hpp"

class Scenario2 : public Scenario {
    public:
        Scenario2(Graph& g);
        void initialize() override;
        void solve() override;
    
    private:
      struct PathInfo {
        Graph::VertexDescriptor base;
        Graph::VertexDescriptor target;
        std::vector<std::string> cities;
        int spyCount;
        double distance;
      };
        void findPaths();
        void buildBaseToPathsMap();

        std::vector<Graph::VertexDescriptor> baseVertices;
        std::vector<Graph::VertexDescriptor> targetVertices;
        std::unordered_map<Graph::VertexDescriptor, std::vector<PathInfo>> baseToPathsMap;
        std::vector<PathInfo> paths; 
};