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
        std::unordered_map<std::string, std::pair<int, int>> missileInventory; // <type, <count, damage>>
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
        if (paths.empty())
            continue;

        std::unordered_map<std::string, std::pair<int, int>> exposedMissiles;

        for (const auto &[missile, count] : missiles)
        {
            const auto &bestPath = paths.front();
            if (bestPath.spyCount < missile.getStealth() && missile.getOveralDistance() >= bestPath.distance)
            {
                std::cout << "Shoot " << count << " " << missile.getType() << " missiles using this safe path:\n";
                for (const auto &cityName : bestPath.cities)
                    std::cout << cityName << " ";
                std::cout << "\n";

                int damage = missile.getDestruction() * count;
                std::cout << "Damage of this attack: " << damage << "\n";
                totalDamage += damage;
            }
            else if (bestPath.spyCount >= missile.getStealth() && missile.getOveralDistance() >= bestPath.distance)
            {
                std::ostringstream oss;
                oss << missile.getType();
                std::string type = oss.str();
                exposedMissiles[type].first += count;
                exposedMissiles[type].second = missile.getDestruction();
            }
        }

        if (!exposedMissiles.empty())
        {
            fallbackBases.push_back({base, baseCity->getName(), std::move(exposedMissiles), paths});
        }
    }

    while (true)
    {
        bool missilesLeft = false;
        for (const auto &fb : fallbackBases)
        {
            for (const auto &[_, data] : fb.missileInventory)
            {
                if (data.first > 0)
                {
                    missilesLeft = true;
                    break;
                }
            }
            if (missilesLeft)
                break;
        }
        if (!missilesLeft)
            break;

        std::unordered_map<std::string, int> totalMissilesToTarget;
        std::unordered_map<std::string, int> totalDamageToTarget;
        std::unordered_map<std::string, std::vector<int>> baseIndexes;

        for (size_t i = 0; i < fallbackBases.size(); ++i)
        {
            const auto &fb = fallbackBases[i];
            for (const auto &path : fb.paths)
            {
                std::string targetName = citiesGraph[path.target]->getName();
                int totalMissileCount = 0, totalDamage = 0;

                for (const auto &[type, data] : fb.missileInventory)
                {
                    totalMissileCount += data.first;
                    totalDamage += data.first * data.second;
                }

                if (totalMissileCount > 0)
                {
                    totalMissilesToTarget[targetName] += totalMissileCount;
                    totalDamageToTarget[targetName] += totalDamage;
                    baseIndexes[targetName].push_back(i);
                }
            }
        }

        if (totalMissilesToTarget.empty())
            break;

        std::string bestTarget;
        int bestTargetDefense = 0;
        int maxBypassedDamage = -1;

        for (const auto &[targetName, count] : totalMissilesToTarget)
        {
            auto it = nameToDesc.find(targetName);
            if (it == nameToDesc.end())
                continue;

            auto tgtCity = std::dynamic_pointer_cast<TargetCity>(citiesGraph[it->second]);
            int defense = tgtCity->getDefenseLevel();
            int bypassed = std::max(0, count - defense);
            int damage = totalDamageToTarget[targetName];
            int bypassedDamage = (count > 0) ? (bypassed * damage / count) : 0;

            if (bypassedDamage > maxBypassedDamage)
            {
                maxBypassedDamage = bypassedDamage;
                bestTarget = targetName;
                bestTargetDefense = defense;
            }
        }

        std::cout << "\n*** Rapid-Fire on \"" << bestTarget << "\" ***\n";
        std::cout << "defense: " << bestTargetDefense << "\n";

        int totalFired = 0, totalBlocked = 0;
        std::unordered_map<std::string, int> blockedByType;

        for (int idx : baseIndexes[bestTarget])
        {
            auto &fb = fallbackBases[idx];
            PathInfo *pathToUse = nullptr;
            for (auto &p : fb.paths)
            {
                if (citiesGraph[p.target]->getName() == bestTarget)
                {
                    pathToUse = &p;
                    break;
                }
            }
            if (!pathToUse)
                continue;

            std::cout << "Base " << fb.baseName << " -> ";
            for (const auto &city : pathToUse->cities)
                std::cout << city << " ";
            std::cout << "\n";

            std::vector<std::pair<std::string, std::pair<int, int>>> sortedMissiles(fb.missileInventory.begin(), fb.missileInventory.end());
            std::sort(sortedMissiles.begin(), sortedMissiles.end(), [](const auto &a, const auto &b)
                      {
                          return a.second.second > b.second.second; // sort by damage descending
                      });

            int baseFired = 0;
            for (auto &[type, data] : sortedMissiles)
            {
                int count = data.first;
                int damage = data.second;
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
                        totalDamage += damage;
                        std::cout << "  ✅ Hit with " << type << " (+" << damage << " damage)\n";
                    }
                    totalFired++;
                }

                baseFired += count;
                fb.missileInventory[type].first = 0;
            }

            if (baseFired == 0)
                std::cout << "  (no missiles left)\n";
        }

        std::cout << "total Missiles Fired: " << totalFired << "\n";
        std::cout << "blocked: " << totalBlocked << "\n";
        std::cout << "bypassed: " << totalFired - totalBlocked << "\n";
        std::cout << "blocked summary:\n";
        for (const auto &[type, cnt] : blockedByType)
            std::cout << "  - " << type << ": " << cnt << "\n";
        std::cout << "hit summary:\n";
        for (const auto &[target, damage] : totalDamageToTarget)
            std::cout << "  - " << target << ": " << damage << "\n";
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