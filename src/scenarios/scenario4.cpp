#include "scenario4.hpp"
#include "missile_type.hpp"
#include "base_city.hpp"
#include <iostream>

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
        missileToGraphs[m] = g;
    }
}

void Scenario4::solve()
{
    initialize();
}