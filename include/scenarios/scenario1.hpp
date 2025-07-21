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
    void solve() override;

    const std::vector<PathInfo>& getPaths() const;
    const std::vector<Graph::VertexDescriptor> getBaseVertices() const;
    const std::vector<Graph::VertexDescriptor> getTargetVertices() const;
    void buildBaseToPathsMap();
    const std::vector<PathInfo>& getPathsFromBase(Graph::VertexDescriptor base) const;
    const void attack();

    private:
    std::vector<Graph::VertexDescriptor> baseVertices;
    std::vector<Graph::VertexDescriptor> targetVertices;

    // map : base -> vector of paths that start from that base
    std::unordered_map<Graph::VertexDescriptor, std::vector<PathInfo>> baseToPathsMap;

    std::vector<PathInfo> paths; 

};