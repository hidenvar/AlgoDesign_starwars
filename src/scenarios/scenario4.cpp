#include "scenario4.hpp"
#include "missile_type.hpp"
#include "base_city.hpp"
#include <iostream>

Scenario4::Scenario4(Graph &g) : Scenario(g) {}

void Scenario4::initialize()
{
    const auto &citiesGraph = Scenario::mapInformation.getCitiesGraph();
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

    std::set<MissileType> missileTypes;

    for (const auto &base : baseVertices)
    {
        const auto &cityPtr = citiesGraph[base];
        auto baseCity = std::dynamic_pointer_cast<BaseCity>(cityPtr);
        for (const auto &[missile, count] : baseCity->getMissiles())
        {
            missileTypes.insert(missile.getType());
        }
    }
}

void Scenario4::solve()
{
    initialize();
}