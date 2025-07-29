#include <queue>
#include <iostream>
#include "base_city.hpp"
#include "target_city.hpp"
#include "scenario2.hpp"

Scenario2::Scenario2(Graph &g) : Scenario(g) {}

void Scenario2::initialize()
{
    int uncontrolledDistance = 500;

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
                Scenario::mapInformation.connectCities(a, b);
            }
        }
    }
}

void Scenario2::findPaths()
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

    paths.clear();

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

                for (size_t i{}; i < currentPath.size(); ++i)
                {
                    const auto &city = citiesGraph[currentPath[i]];
                    pathInfo.cities.push_back(city->getName());
                    if (city->hasSpy())
                        spyCount++;

                    if (i > 0)
                    {
                        const auto &prevCity = citiesGraph[currentPath[i - 1]];
                        totalDistance += Scenario::mapInformation.calculateDistance(*prevCity, *city);
                    }
                }

                pathInfo.distance = totalDistance;
                pathInfo.spyCount = spyCount;
                paths.push_back(pathInfo);
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

void Scenario2::buildBaseToPathsMap()
{
    baseToPathsMap.clear();
    auto compare = [](const PathInfo &a, const PathInfo &b)
    {
        return std::tie(a.spyCount, a.distance) < std::tie(b.spyCount, b.distance);
    };

    for (const auto &path : paths)
    {
        baseToPathsMap[path.base].push_back(path);
    }

    for (auto &[base, vec] : baseToPathsMap)
    {
        sort(vec.begin(), vec.end(), compare);
    }
}

void Scenario2::attack()
{
    int totalDamage = 0;
    const auto &citiesGraph = Scenario::mapInformation.getCitiesGraph();
    const auto &nameToDesc = mapInformation.getCitiesVertex();

    struct FallbackBase
    {
        Graph::VertexDescriptor base;
        std::string baseName;
        std::unordered_map<std::string, int> missileInventory;
        int damagePerMissile;
        std::vector<PathInfo> paths;
    };

    std::vector<FallbackBase> fallbackBases;

    for (const auto &base : baseVertices)
    {

        const auto &cityPtr = citiesGraph[base];
        auto baseCity = std::dynamic_pointer_cast<BaseCity>(cityPtr);
        if (!baseCity)
            continue;

        const auto &missiles = baseCity->getMissiles();
        const auto &paths = baseToPathsMap[base];
        bool hasSafePath = false;

        const auto &bestPath = paths.front();

        for (const auto &[missile, count] : missiles)
        {
            if (!paths.empty() && bestPath.spyCount < missile.getStealth() && missile.getOveralDistance() >= bestPath.distance)
            {

                hasSafePath = true;
                std::cout << "Shoot " << count << " " << missile.getType()
                          << " missiles using this safe path:\n";

                for (const auto &cityName : bestPath.cities)
                    std::cout << cityName << " ";
                std::cout << "\n";

                int damage = missile.getDestruction() * count;
                std::cout << "Damage of this attack: " << damage << "\n";
                totalDamage += damage;
            }
        }

        if (!hasSafePath && !paths.empty())
        {

            FallbackBase fallbackBase;
            fallbackBase.base = base;
            fallbackBase.baseName = baseCity->getName();
            fallbackBase.paths = paths;

            for (const auto &[missile, count] : missiles)
            {
                std::ostringstream oss;
                oss << missile.getType();
                fallbackBase.missileInventory[oss.str()] += count;
                fallbackBase.damagePerMissile = missile.getDestruction();
            }
            fallbackBases.push_back(fallbackBase);
        }
    }

    while (true)
    {
        // Check if any missiles remain in fallback bases
        bool missileAvailable = std::any_of(fallbackBases.begin(), fallbackBases.end(), [](const auto &base)
                                            { return std::any_of(base.missileInventory.begin(), base.missileInventory.end(), [](const auto &pair)
                                                                 { return pair.second > 0; }); });
        if (!missileAvailable)
            break;

        std::unordered_map<std::string, int> totalMissilesToTarget;
        std::unordered_map<std::string, int> damagePerMissile;
        std::unordered_map<std::string, std::vector<int>> baseIndexes;

        // Aggregate missile data per target
        for (int i = 0; i < fallbackBases.size(); ++i)
        {
            const auto &base = fallbackBases[i];
            for (const auto &path : base.paths)
            {
                std::string targetName = citiesGraph[path.target]->getName();
                int missileCount = 0;
                for (const auto &[type, qty] : base.missileInventory)
                    missileCount += qty;
                if (missileCount > 0)
                {
                    totalMissilesToTarget[targetName] += missileCount;
                    damagePerMissile[targetName] = base.damagePerMissile;
                    baseIndexes[targetName].push_back(i);
                }
            }
        }

        if (totalMissilesToTarget.empty())
            break;

        // Find best target with maximum effective damage
        std::string bestTarget;
        int bestTargetDefense = 0;
        int maxBypassedDamage = -1;
        for (const auto &[targetName, missileCount] : totalMissilesToTarget)
        {
            auto it = nameToDesc.find(targetName);
            if (it == nameToDesc.end())
                continue;
            auto tgtCity = std::dynamic_pointer_cast<TargetCity>(citiesGraph[it->second]);
            int defense = tgtCity->getDefenseLevel();
            int damage = std::max(0, missileCount - defense) * damagePerMissile[targetName];
            if (damage > maxBypassedDamage)
            {
                maxBypassedDamage = damage;
                bestTarget = targetName;
                bestTargetDefense = defense;
            }
        }

        std::cout << "\n*** Rapid-Fire on \"" << bestTarget << "\" ***\n";
        std::cout << "defense: " << bestTargetDefense << "\n";

        int totalFired = 0, totalBlocked = 0;
        std::unordered_map<std::string, int> blockedByType;

        // Fire missiles at best target, prioritizing most damaging types first
        for (int idx : baseIndexes[bestTarget])
        {
            auto &base = fallbackBases[idx];
            auto pathIt = std::find_if(base.paths.begin(), base.paths.end(), [&](const PathInfo &p)
                                       { return citiesGraph[p.target]->getName() == bestTarget; });
            if (pathIt == base.paths.end())
                continue;

            std::cout << "Base " << base.baseName << " -> ";
            for (const auto &city : pathIt->cities)
                std::cout << city << " ";
            std::cout << "\n";

            int baseFired = 0;

            // Sort missiles by damage descending (assuming missileTypes have associated damage)
            std::vector<std::pair<std::string, int>> sortedMissiles(base.missileInventory.begin(), base.missileInventory.end());
            std::sort(sortedMissiles.begin(), sortedMissiles.end(), [&](const auto &a, const auto &b)
                      { return base.missileInventory.at(a.first) > base.missileInventory.at(b.first); });

            for (auto &[type, count] : sortedMissiles)
            {
                if (count == 0)
                    continue;
                std::cout << "  - " << count << " x " << type << "\n";

                for (int i = 0; i < count; ++i)
                {
                    if (totalBlocked < bestTargetDefense)
                    {
                        totalBlocked++;
                        blockedByType[type]++;
                    }
                    else
                    {
                        totalDamage += base.missileInventory.at(type);
                    }
                    totalFired++;
                }

                baseFired += count;
                base.missileInventory[type] = 0;
            }

            if (baseFired == 0)
                std::cout << "  (no missiles left)\n";
        }

        std::cout << "total Missiles Fired: " << totalFired << "\n";
        std::cout << "blocked: " << totalBlocked << "\n";
        std::cout << "bypassed: " << totalFired - totalBlocked << "\n";
        std::cout << "blocked summary:\n";
        for (const auto &[type, count] : blockedByType)
            std::cout << "  - " << type << ": " << count << "\n";
    }
    std::cout << "\n************\nTotal Damage: " << totalDamage << "\n";
}

void Scenario2::solve()
{
    initialize();
    findPaths();
    buildBaseToPathsMap();
    attack();
    // printAllCities();
}