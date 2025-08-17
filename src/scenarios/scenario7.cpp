#include <queue>

#include "scenario7.hpp"
#include "graph.hpp"

void Scenario7::initialize()
{
    int uncontrolledDistance = 900;

    auto citiesGraph = Scenario::mapInformation.getCitiesGraph();
    auto cityVertices = Scenario::mapInformation.getCitiesVertex();

    // convert to vector
    std::vector<std::pair<std::string, Graph::VertexDescriptor>> cities(
        cityVertices.begin(), cityVertices.end());

    // iterate over all cities pairs
    for (size_t i = 0; i < cities.size(); ++i)
    {
        for (size_t j = i + 1; j < cities.size(); ++j)
        {
            const std::string &city1Name = cities[i].first;
            const std::string &city2Name = cities[j].first;

            auto city1 = citiesGraph[cities[i].second];
            auto city2 = citiesGraph[cities[j].second];

            // calculate distance
            double distance =
                Scenario::mapInformation.calculateDistance(*city1, *city2);

            // connect  if distance <= uncontrolledDistance
            if (distance <= uncontrolledDistance)
            {
                std::string a = city1Name;
                std::string b = city2Name;
                // add edge between cities
                Scenario::mapInformation.connectCities(a, b);
                // set edge weight
                Scenario::mapInformation.setDistanceBetweenCities(a, b, distance);
            }
        }
    }
}

void Scenario7::findPaths()
{
    const auto &citiesGraph = Scenario::mapInformation.getCitiesGraph();

    // Identify base and target cities
    auto vertices = boost::vertices(citiesGraph);
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

    paths.clear(); // Clear previous results

    // BFS for each base city
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

            bool isTarget = (std::find(targetVertices.begin(), targetVertices.end(),
                                       currentVertex) != targetVertices.end());
            if (isTarget)
            {
                PathInfo pathInfo;
                pathInfo.base = base;
                pathInfo.target = currentVertex;
                int spyCount = 0;
                double maxGap = 0.0;
                double totalDistance = 0.0;

                const auto &weightMap = boost::get(boost::edge_weight, citiesGraph);

                for (size_t i = 0; i < currentPath.size(); ++i)
                {
                    const auto &city = citiesGraph[currentPath[i]];
                    pathInfo.cities.push_back(city->getName());
                    if (city->hasSpy())
                    {
                        spyCount++;
                    }

                    if (i > 0)
                    {
                        const auto &prevCity = citiesGraph[currentPath[i - 1]];
                        totalDistance +=
                            Scenario::mapInformation.calculateDistance(*prevCity, *city);

                        auto [e, exists] =
                            boost::edge(currentPath[i - 1], currentPath[i], citiesGraph);
                        if (exists)
                        {
                            maxGap = std::max(maxGap, (weightMap[e]));
                        }
                    }
                }

                pathInfo.distance = totalDistance;
                pathInfo.spyCount = spyCount;
                pathInfo.max_gap = maxGap;
                paths.push_back(pathInfo);
            }

            // Explore neighbors
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
