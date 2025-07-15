#pragma once
#include"scenario.hpp"

class Scenario1 : public Scenario{
    public:
    Scenario1(Graph& g);

    void initialize() override;
    void findPaths();
    struct PathInfo {
        std::vector<std::string> cities;
        int spyCount;
    };

    const std::vector<PathInfo>& getPaths() const;
    
    private:
    std::vector<PathInfo> paths; 
    void solve() override {}

};