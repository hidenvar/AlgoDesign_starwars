#pragma once
#include"scenario.hpp"

class Scenario1 : public Scenario{
    public:
    Scenario1(Graph& g);

    void initialize() override;
    void findPaths();
    struct PathInfo {
        Graph::VertexDescriptor base;
        Graph::VertexDescriptor target;
        std::vector<std::string> cities;
        int spyCount;
    };
    void solve() override {}

    const std::vector<PathInfo>& getPaths() const;
    const std::vector<Graph::VertexDescriptor> getBaseVertices() const;
    const std::vector<Graph::VertexDescriptor> getTargetVertices() const;

    private:
    std::vector<Graph::VertexDescriptor> baseVertices;
    std::vector<Graph::VertexDescriptor> targetVertices;

    std::vector<PathInfo> paths; 

};