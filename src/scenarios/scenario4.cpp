#include "scenario4.hpp"
#include "missile_type.hpp"
#include "base_city.hpp"
#include <iostream>
#include <queue>
#include <iomanip> // for std::setprecision

Scenario4::Scenario4(Graph &g) : Scenario(g) {}

void Scenario4::initialize()
{
    auto citiesGraph = Scenario::mapInformation.getCitiesGraph();
    auto cityVertices = Scenario::mapInformation.getCitiesVertex();
    auto vertices = boost::vertices(citiesGraph);
    std::vector<std::pair<std::string, Graph::VertexDescriptor>> cities(
        cityVertices.begin(), cityVertices.end());

    for (auto vit = vertices.first; vit != vertices.second; ++vit)
    {
        const auto &city = citiesGraph[*vit];
        if (city->getType() == CityType::BASE)
        {
            baseVertices.push_back(*vit);
        }
        else if (city->getType() == CityType::TARGET)
        {
            targetVertices.push_back(*vit);
        }
    }

    std::set<int> missileVariations;

    for (const auto &base : baseVertices)
    {
        const auto &cityPtr = citiesGraph[base];
        auto baseCity = std::dynamic_pointer_cast<BaseCity>(cityPtr);
        for (const auto &[missile, count] : baseCity->getMissiles())
        {
            missileVariations.insert(missile.getUncontrolledDistance());
        }
    }

    std::vector<int> missilesUncontrolledDistances(missileVariations.begin(), missileVariations.end());

    for (const auto &m : missilesUncontrolledDistances)
    {
        Graph g = Scenario::mapInformation;
        for (size_t i = 0; i < cities.size(); ++i)
        {
            for (size_t j = i + 1; j < cities.size(); ++j)
            {
                const std::string &city1Name = cities[i].first;
                const std::string &city2Name = cities[j].first;

                auto city1 = citiesGraph[cities[i].second];
                auto city2 = citiesGraph[cities[j].second];

                double distance =
                    Scenario::mapInformation.calculateDistance(*city1, *city2);

                if (distance <= m)
                {
                    std::string a = city1Name;
                    std::string b = city2Name;
                    g.connectCities(a, b);
                }
            }
        }
        missileToGraphs[m].first = g;
    }
}

void Scenario4::findPaths()
{
    for (auto &[key, val] : missileToGraphs)
    {
        const auto &citiesGraph = val.first.getCitiesGraph();
        val.second.clear();

        for (auto base : baseVertices)
        {
            std::queue<std::vector<Graph::VertexDescriptor>> q;
            std::unordered_set<Graph::VertexDescriptor> visited;

            q.push({base});
            visited.insert(base);

            while (!q.empty())
            {
                auto currentPath = q.front();
                q.pop();
                auto currentVertex = currentPath.back();

                // Check if reached target
                bool isTarget = (std::find(targetVertices.begin(), targetVertices.end(), currentVertex) != targetVertices.end());
                if (isTarget)
                {
                    // Convert path to city names and count spies
                    PathInfo pathInfo;
                    pathInfo.base = base;
                    pathInfo.target = currentVertex;
                    int spyCount = 0;
                    double totalDistance = 0.0;
                    double maxGap = INT_MIN;

                    for (size_t i{}; i < currentPath.size(); ++i)
                    {
                        const auto &city = citiesGraph[currentPath[i]];
                        pathInfo.cities.push_back(city->getName());
                        if (city->hasSpy())
                            spyCount++;

                        if (i > 0)
                        {
                            const auto &prevCity = citiesGraph[currentPath[i - 1]];
                            auto td = val.first.calculateDistance(*prevCity, *city);
                            maxGap = std::max(maxGap, td);
                            totalDistance += td;
                        }
                    }

                    pathInfo.totalDistance = totalDistance;
                    pathInfo.spyCount = spyCount;
                    pathInfo.maxGap = maxGap;
                    val.second.push_back(pathInfo);
                }

                auto neighbors = boost::adjacent_vertices(currentVertex, citiesGraph);
                for (auto nit = neighbors.first; nit != neighbors.second; ++nit)
                {
                    auto neighbor = *nit;

                    if (visited.find(neighbor) == visited.end())
                    {
                        visited.insert(neighbor);
                        auto newPath = currentPath;
                        newPath.push_back(neighbor);
                        q.push(newPath);
                    }
                }
            }
        }
    }
}

void Scenario4::buildPaths()
{
    auto compare = [](const PathInfo &a, const PathInfo &b)
    {
        return std::tie(a.spyCount, a.maxGap, a.totalDistance) < std::tie(b.spyCount, b.maxGap, b.totalDistance);
    };

    for (const auto &[missileUn, graphPair] : missileToGraphs)
    {
        missilePathsByBase[missileUn].clear();

        for (const auto &path : graphPair.second)
        {
            missilePathsByBase[missileUn][path.base].push_back(path);
        }

        for (auto &[base, paths] : missilePathsByBase[missileUn])
        {
            sort(paths.begin(), paths.end(), compare);
        }
    }
}

void Scenario4::solve()
{
    initialize();
    findPaths();
    buildPaths();
    printPathInfo();
}



void Scenario4::printPathInfo() const
{
    std::cout << "=== Missile Path Info ===\n";

    for (const auto &[missileUn, basePaths] : missilePathsByBase)
    {
        std::cout << "Missile Uncontrolled Distance: " << missileUn << "\n";

        for (const auto &[base, paths] : basePaths)
        {
            std::cout << "  Base: " << Scenario::mapInformation.getCitiesGraph()[base]->getName() << "\n";

            for (const auto &path : paths)
            {
                std::cout << "    Path: ";
                for (const auto &cityName : path.cities)
                {
                    std::cout << cityName << " -> ";
                }
                std::cout << "END\n";

                std::cout << "      Spy Count: " << path.spyCount
                          << ", Max Gap: " << std::fixed << std::setprecision(2) << path.maxGap
                          << ", Total Distance: " << path.totalDistance << "\n";
            }
        }

        std::cout << "-------------------------\n";
    }
}
