#include "scenario4.hpp"
#include "missile_type.hpp"
#include "base_city.hpp"
#include <iostream>
#include <queue>
#include <iomanip> // for std::setprecision
#include "target_city.hpp"

Scenario4::Scenario4(Graph &g) : Scenario(g) {}

void Scenario4::initialize()
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
                Scenario::mapInformation.connectCities(a, b);
            }
        }
    }
}

void Scenario4::findPaths()
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
                        totalDistance += Scenario::mapInformation.calculateDistance(*prevCity, *city);

                        auto [e, exists] =
                            boost::edge(currentPath[i - 1], currentPath[i], citiesGraph);
                        if (exists)
                        {
                            maxGap = std::max(maxGap, (weightMap[e]));
                        }
                    }
                }

                pathInfo.totalDistance = totalDistance;
                pathInfo.spyCount = spyCount;
                pathInfo.maxGap = maxGap;
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

void Scenario4::buildPaths()
{
    missilePathMap.clear();

    for (const auto &path : paths)
    {
        // Helper lambda to insert if better
        auto insertOrUpdate = [&](const std::string &key, Graph::VertexDescriptor base,
                                  const PathInfo &candidate)
        {
            auto &basePaths = missilePathMap[key][base];
            if (basePaths.empty() || key.find("revealed") != std::string::npos)
            {
                // No path stored yet, add candidate
                basePaths.push_back(candidate);
            }
            else
            {
                // Compare candidate with stored path, keep the better one
                const auto &stored = basePaths.front();

                if (candidate.spyCount < stored.spyCount)
                {
                    basePaths[0] = candidate;
                }
            }
        };

        Graph::VertexDescriptor base = path.base;

        // A type missiles
        if (path.totalDistance <= 2500 && path.maxGap <= 500)
        {
            insertOrUpdate(path.spyCount < 3 ? "A1 safe" : "A1 revealed", base, path);
            insertOrUpdate(path.spyCount < 2 ? "A2 safe" : "A2 revealed", base, path);
            insertOrUpdate(path.spyCount < 4 ? "A3 safe" : "A3 revealed", base, path);
        }

        // B type missiles
        if (path.totalDistance <= 5000 && path.maxGap <= 500)
        {
            insertOrUpdate(path.spyCount < 2 ? "B1 safe" : "B1 revealed", base, path);
            insertOrUpdate(path.spyCount < 0 ? "B2 safe" : "B2 revealed", base, path);
        }

        // C1 type missiles
        if (path.totalDistance <= 3000 && path.maxGap <= 700)
        {
            insertOrUpdate(path.spyCount < 2 ? "C1 safe" : "C1 revealed", base, path);
        }

        // C2 type missiles
        if (path.totalDistance <= 2900 && path.maxGap <= 900)
        {
            insertOrUpdate(path.spyCount < 1 ? "C2 safe" : "C2 revealed", base, path);
        }
    }
}

void Scenario4::attack()
{
    int totalDamage = 0;
    const auto &citiesGraph = Scenario::mapInformation.getCitiesGraph();
    const auto &nameToDesc = mapInformation.getCitiesVertex();

    struct FallbackBase
    {
        Graph::VertexDescriptor base;
        std::string baseName;
        std::unordered_map<std::string, std::pair<int, int>> missileInventory;
        std::vector<PathInfo> paths;
    };
    std::vector<FallbackBase> fallbackBases;

    for (const auto &base : baseVertices)
    {

        const auto &baseCity = citiesGraph[base];
        BaseCity *baseCityPtr = dynamic_cast<BaseCity *>(baseCity.get());

        if (!baseCityPtr)
            continue;

        std::unordered_map<std::string, std::pair<int, int>> exposedMissiles;

        for (const auto &[missile, count] : baseCityPtr->getMissiles())
        {
            const auto &missileType = missile.getTypeString();
            const auto &safePaths = missilePathMap[missileType + " safe"][base];
            const auto &revealedPaths = missilePathMap[missileType + " revealed"][base];

            if (!safePaths.empty())
            {

                // Use the first path for the attack
                const auto &path = safePaths.front();
                std::cout << "Launching " << count << " " << missile.getType() << " missiles using this path:\n";
                for (const auto &cityName : path.cities)
                {
                    std::cout << cityName << " -> ";
                }
                std::cout << "END\n";

                int damage = missile.getDestruction() * count;
                std::cout << "Damage of this attack: " << damage << "\n";
                totalDamage += damage;
            }

            if (!revealedPaths.empty())
            {
                exposedMissiles[missileType].first = count;
                exposedMissiles[missileType].second = missile.getDestruction();
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
    }

    std::cout << "\n************\nTotal Damage: " << totalDamage << "\n";
}

void Scenario4::solve()
{
    initialize();
    findPaths();
    buildPaths();
    attack();
    // printPathInfo();
}


void Scenario4::printPathInfo() const
{
    std::cout << "=== Missile Path Info by Type ===\n";

    const std::vector<std::string> expectedCategories = {
        "A1 safe", "A1 revealed",
        "A2 safe", "A2 revealed",
        "A3 safe", "A3 revealed",
        "B1 safe", "B1 revealed",
        "B2 safe", "B2 revealed",
        "C1 safe", "C1 revealed",
        "C2 safe", "C2 revealed"};

    for (const auto &category : expectedCategories)
    {
        std::cout << "Missile Type: " << category << "\n";

        auto it = missilePathMap.find(category);
        if (it == missilePathMap.end() || it->second.empty())
        {
            std::cout << "  No paths available for this category.\n";
        }
        else
        {
            // it->second is unordered_map<Graph::VertexDescriptor, std::vector<PathInfo>>
            for (const auto &[baseVertex, paths] : it->second)
            {
                std::cout << "  Base vertex: " << baseVertex << "\n";

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
        }
        std::cout << "-------------------------\n";
    }
}
