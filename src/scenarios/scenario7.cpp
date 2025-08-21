#include <queue>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "scenario7.hpp"
#include "graph.hpp"
#include "missile_factory.hpp"
#include "scenario7_input.hpp"
#include "target_city.hpp"

Scenario7::Scenario7() {}

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

void Scenario7::buildMissilePathMap()
{
    missilePathMap.clear(); // Clear existing entries

    // Define all missile types with their identifiers
    static const std::vector<std::pair<std::string, Missile>> missileList = {
        {"A1", MissileFactory::A1}, {"A2", MissileFactory::A2}, {"A3", MissileFactory::A3}, {"B1", MissileFactory::B1}, {"B2", MissileFactory::B2}, {"C1", MissileFactory::C1}, {"C2", MissileFactory::C2}, {"D1", MissileFactory::D1}};

    for (auto &path : paths)
    {
        // Check against each missile type
        for (const auto &missilePair : missileList)
        {
            const std::string &missileType = missilePair.first;
            const Missile &missile = missilePair.second;

            // Check range and gap
            if (path.distance <= missile.getOveralDistance() &&
                path.max_gap <= missile.getUncontrolledDistance())
            {
                // safe/revealed status
                if (path.spyCount < missile.getStealth())
                {
                    missilePathMap[missileType + " safe"].push_back(path);
                }
                else
                {
                    missilePathMap[missileType + " revealed"].push_back(path);
                }
            }
        }
    }

    std::cout << "\n======= Summary =======\n";
    const std::vector<std::string> missileOrder = {
        "A1 safe", "A1 revealed", "A2 safe", "A2 revealed",
        "A3 safe", "A3 revealed", "B1 safe", "B1 revealed",
        "B2 safe", "B2 revealed", "C1 safe", "C1 revealed",
        "C2 safe", "C2 revealed", "D1 safe", "D1 revealed"};
    for (const auto &category : missileOrder)
    {
        auto it = missilePathMap.find(category);
        if (it != missilePathMap.end())
        {
            std::cout << category << ": " << it->second.size() << " paths\n";
        }
        else
        {
            std::cout << category << ": 0 paths\n";
        }
    }
}

void Scenario7::solve()
{
    Scenario7Input::fillInventory(std::cin, inventory, missilePrice);
    Scenario7Input::createCities(std::cin, Scenario::mapInformation);
    Scenario7Input::setDesiredDamage(std::cin, desiredDamage);
    initialize();
    findPaths();
    buildMissilePathMap();
    attack();
}

int Scenario7::findMinimumCost(std::unordered_map<std::string, std::pair<int, double>> &prices, std::map<std::string, int> &usedMissiles)
{
    const int INF = INT_MAX / 2;
    int maxD = desiredDamage + 1000;
    std::vector<int> dp(maxD + 1, INF);
    std::vector<std::map<std::string, int>> choice(maxD + 1);
    dp[0] = 0;

    for (const auto &[name, info] : prices)
    {
        int count = info.first;
        int price = info.second;
        auto damage = MissileFactory::getMissile(getMissileType(name)).getDestruction();
        for (int k = 1; count > 0; k <<= 1)
        {
            int use = std::min(k, count);
            int total_d = use * damage;
            int total_p = use * price;

            for (int j = maxD - total_d; j >= 0; --j)
            {
                if (dp[j] != INF && dp[j + total_d] > dp[j] + total_p)
                {
                    dp[j + total_d] = dp[j] + total_p;
                    choice[j + total_d] = choice[j];
                    choice[j + total_d][name] += use;
                }
            }

            count -= use;
        }
    }

    int result = INF;
    int bestD = -1;
    for (int i = desiredDamage; i <= maxD; ++i)
    {
        if (dp[i] < result)
        {
            result = dp[i];
            bestD = i;
        }
    }

    if (result == INF)
        return -1;

    usedMissiles = choice[bestD];
    return result;
}

void Scenario7::attack()
{
 // Safe Attack
 
    const auto &citiesGraph = Scenario::mapInformation.getCitiesGraph();
    std::unordered_map<std::string, std::pair<int, double>> safeMissiles;
    std::map<std::string, int> usedMissiles;
    int totalDamage = 0;
    int totalCost = 0;
    int successfulNights = 0;
    std::vector<std::string> nightResults;

    int totalNights = 7;
    for (int night = 1; night <= totalNights; ++night)
    {

        safeMissiles.clear();
        for (const auto &[missile, cnt] : missilePrice)
        {
            if (!missilePathMap[missile + " safe"].empty())
            {
                safeMissiles[missile] = {cnt.first, cnt.second};
            }
        }

        int nightDamage = 0;
        int nightCost = 0;
        int minimumCost = findMinimumCost(safeMissiles, usedMissiles);
        if (minimumCost != -1)
        {
            // Safe Attack
            std::cout << "\n===================== Night " << night << " Attack Summary =====================\n";
            std::cout << std::left
                      << std::setw(15) << "Missile Type"
                      << std::setw(12) << "Count"
                      << std::setw(12) << "Damage"
                      << std::setw(12) << "Cost"
                      << "Path\n";
            std::cout << std::string(80, '-') << '\n';

            for (const auto &[missile, cnt] : usedMissiles)
            {
                int missileDmg = MissileFactory::getMissile(getMissileType(missile)).getDestruction() * cnt;
                int missileCost = missilePrice[missile].second * cnt;
                nightDamage += missileDmg;
                nightCost += missileCost;

                const auto &cityVec = missilePathMap[missile + " safe"];
                const auto &path = cityVec[0].cities;

                std::stringstream pathStream;
                for (size_t i = 0; i < path.size(); ++i)
                {
                    pathStream << path[i];
                    if (i != path.size() - 1)
                        pathStream << " -> ";
                }

                std::cout << std::left
                          << std::setw(15) << missile
                          << std::setw(12) << ("x" + std::to_string(cnt))
                          << std::setw(12) << missileDmg
                          << std::setw(12) << missileCost
                          << pathStream.str() << '\n';
            }

            std::cout << std::string(80, '-') << '\n';
            std::cout << "Total damage this round : " << nightDamage << '\n';
            std::cout << "Total cost   this round : " << nightCost << '\n';
        }
        else
        {
            // =================== Fallback Attack ===================

            int bestCost = INT_MAX;
            std::map<std::string, int> bestUsedMissiles;
            std::string bestTargetName;

            for (const auto &target : targetVertices)
            {
                safeMissiles.clear();
                std::vector<Missile> exposed;
                std::vector<Missile> unexposed;
                std::unordered_set<std::string> addedMissiles;

                // Collect usable missiles from both safe and revealed paths
                for (const auto &[missileName, missileInfo] : missilePrice)
                {
                    bool added = false;

                    for (const std::string status : {" safe", " revealed"})
                    {
                        std::string key = missileName + status;
                        if (missilePathMap.count(key) == 0)
                            continue;

                        for (const auto &path : missilePathMap[key])
                        {
                            if (path.target == target && !addedMissiles.count(missileName))
                            {
                                Missile missile = MissileFactory::getMissile(getMissileType(missileName));
                                if (status == " revealed")
                                    exposed.push_back(missile);
                                else
                                    unexposed.push_back(missile);
                                addedMissiles.insert(missileName);
                                added = true;
                                break;
                            }
                        }

                        if (added)
                            break;
                    }
                }

                // Sort exposed (revealed) missiles to breach defense with highest damage first
                std::sort(exposed.begin(), exposed.end(), [](const Missile &a, const Missile &b)
                          { return a.getDestruction() > b.getDestruction(); });

                auto targetCity = citiesGraph[target];
                auto targetPtr = std::dynamic_pointer_cast<TargetCity>(targetCity);
                if (!targetPtr)
                    continue;

                size_t defenseLevel = targetPtr->getDefenseLevel();
                size_t i = 0;

                // Use exposed missiles to breach defense
                while (defenseLevel > 0 && i < exposed.size())
                {
                    const auto &missileType = exposed[i].getTypeString();
                    int missileCount = missilePrice[missileType].first;
                    int used = std::min(static_cast<int>(defenseLevel), missileCount);
                    int remaining = missileCount - used;

                    if (remaining > 0)
                    {
                        safeMissiles[missileType] = {remaining, missilePrice[missileType].second};
                    }

                    defenseLevel -= used;
                    ++i;
                }

                // Add remaining exposed missiles (unused for breaching)
                while (i < exposed.size())
                {
                    const auto &missileType = exposed[i].getTypeString();
                    safeMissiles[missileType] = {missilePrice[missileType].first, missilePrice[missileType].second};
                    ++i;
                }

                // Add all unexposed missiles
                for (const auto &missile : unexposed)
                {
                    const auto &missileType = missile.getTypeString();
                    safeMissiles[missileType] = {missilePrice[missileType].first, missilePrice[missileType].second};
                }

                // Find minimum cost with new inventory
                usedMissiles.clear();
                minimumCost = findMinimumCost(safeMissiles, usedMissiles);
                if (minimumCost != -1 && minimumCost < bestCost)
                {
                    bestCost = minimumCost;
                    bestUsedMissiles = usedMissiles;
                    bestTargetName = targetPtr->getName();
                }
            }

            if (bestCost != INT_MAX)
            {
                std::cout << "\n🛑 Fallback Attack Activated (Night " << night << ")\n";
                std::cout << "Best target: " << bestTargetName << "\n";
                std::cout << std::left
                          << std::setw(15) << "Missile Type"
                          << std::setw(12) << "Count"
                          << std::setw(12) << "Damage"
                          << std::setw(12) << "Cost"
                          << "Path\n";
                std::cout << std::string(80, '-') << '\n';

                nightDamage = 0;
                nightCost = 0;

                usedMissiles = bestUsedMissiles;
                for (const auto &[missile, cnt] : bestUsedMissiles)
                {
                    int damage = MissileFactory::getMissile(getMissileType(missile)).getDestruction() * cnt;
                    int cost = missilePrice[missile].second * cnt;

                    nightDamage += damage;
                    nightCost += cost;

                    const std::string pathKey = missile + (!missilePathMap[missile + " safe"].empty() ? " safe" : " revealed");
                    const auto &cityVec = missilePathMap[pathKey];
                    const auto &path = cityVec[0].cities;

                    std::stringstream pathStream;
                    for (size_t i = 0; i < path.size(); ++i)
                    {
                        pathStream << path[i];
                        if (i != path.size() - 1)
                            pathStream << " -> ";
                    }

                    std::cout << std::left
                              << std::setw(15) << missile
                              << std::setw(12) << ("x" + std::to_string(cnt))
                              << std::setw(12) << damage
                              << std::setw(12) << cost
                              << pathStream.str() << '\n';
                }

                std::cout << std::string(80, '-') << '\n';
                std::cout << "Total damage this round : " << nightDamage << '\n';
                std::cout << "Total cost   this round : " << nightCost << '\n';
            }
        }

        // Log successful nights
        bool nightSuccess = nightDamage >= desiredDamage;
        if (nightSuccess)
        {
            std::cout << "✅ Status: SUCCESS — Target damage achieved.\n";
            successfulNights++;
            nightResults.push_back("Night " + std::to_string(night) + ": ✅ SUCCESS");
            totalDamage += nightDamage;
            totalCost += nightCost;

            // Update inventory
            for (const auto &[m, cnt] : usedMissiles)
            {
                missilePrice[m].first -= cnt;
                if (missilePrice[m].first == 0)
                {
                    missilePrice.erase(m);
                }
            }
            continue;
        }
        else
        {
            nightResults.push_back("Night " + std::to_string(night) + ": ❌ FAILED — No solution");
        }
    }

    std::cout << "\n===================== Final Summary =====================\n";
    for (const auto &line : nightResults)
    {
        std::cout << line << '\n';
    }
    std::cout << "\nTotal successful nights : " << successfulNights << "/" << totalNights << '\n';
    std::cout << "Total damage dealt      : " << totalDamage << '\n';
    std::cout << "Total cost spent        : " << totalCost << '\n';
}

// for (const auto &target : targetVertices)
// {
//     std::vector<Missile> exposed;
//     std::vector<Missile> unexposed;

//     // Step 1: Classify missiles for this target
//     for (const auto &[missileStatus, pathVec] : missilePathMap)
//     {
//         const auto &missileName = missileStatus.substr(0, missileStatus.find(' '));
//         bool isExposed = missileStatus.find("revealed") != std::string::npos;

//         if (missilePrice.find(missileName) == missilePrice.end())
//             continue;

//         for (const auto &pathInfo : pathVec)
//         {
//             if (pathInfo.target == target)
//             {
//                 isExposed
//                     ? exposed.push_back(MissileFactory::getMissile(getMissileType(missileName)))
//                     : unexposed.push_back(MissileFactory::getMissile(getMissileType(missileName)));
//             }
//         }
//     }

//     // Step 2: Sort exposed missiles by damage descending
//     std::sort(exposed.begin(), exposed.end(), [](const Missile &a, const Missile &b)
//               { return a.getDestruction() > b.getDestruction(); });

//     // Step 3: Breach defense: remove top defenseLevel missiles
//     int defenseLevel = targetDefense[target];
//     std::vector<Missile> remainingExposed;
//     for (size_t i = 0; i < exposed.size(); ++i)
//     {
//         if (i >= defenseLevel)
//         {
//             remainingExposed.push_back(exposed[i]);
//         }
//     }

//     // Step 4: Combine remaining exposed + unexposed for DP
//     std::vector<Missile> usableMissiles = remainingExposed;
//     usableMissiles.insert(usableMissiles.end(), unexposed.begin(), unexposed.end());

//     // Step 5: Apply DP to find min-cost subset that reaches desired damage
//     auto [cost, selectedMissiles] = dpMinCost(usableMissiles, desiredDamage);

//     // Step 6: Keep track of best solution
//     if (cost != -1 && (bestCost == -1 || cost < bestCost))
//     {
//         bestCost = cost;
//         bestMissiles = selectedMissiles;
//         bestTarget = target;
//     }
// }

// struct MissileInstance
// {
//     std::string missileName;
//     int damage;
//     int cost;
//     bool safe;
//     std::string target;
// };

// std::vector<MissileInstance> survivedMissiles;

// for (auto &target : targets)
// {
//     std::vector<MissileInstance> exposedMissilesToTarget;
//     // gather all exposed missiles for this target
//     for (auto &opt : options)
//     {
//         if (!opt.safe && opt.target == target.name)
//         {
//             for (int i = 0; i < opt.availableCount; ++i)
//                 exposedMissilesToTarget.push_back({opt.missileName, opt.damage, opt.cost, false, target.name});
//         }
//     }

//     // sort by descending damage
//     std::sort(exposedMissilesToTarget.begin(), exposedMissilesToTarget.end(),
//               [](const MissileInstance &a, const MissileInstance &b)
//               { return a.damage > b.damage; });

//     int destroyCount = target.defenseLevel;
//     for (size_t i = 0; i < exposedMissilesToTarget.size(); ++i)
//     {
//         if (destroyCount > 0)
//         {
//             // missile destroyed
//             --destroyCount;
//         }
//         else
//         {
//             survivedMissiles.push_back(exposedMissilesToTarget[i]);
//         }
//     }
// }

// std::map<std::string, TargetStatus> targetMissilesMap;

// for (auto &target : targets) {
//     TargetStatus status;
//     status.targetName = target.name;
//     status.defenseLevel = target.defenseLevel;

//     std::vector<MissileInstance> exposed;

//     // Gather missiles that can reach this target
//     for (auto &[missile, pathVec] : missilePath) {
//         for (auto &pathInfo : pathVec) {
//             if (pathInfo.target == target.name) {
//                 MissileInstance m;
//                 m.missileName = missile;
//                 m.damage = MissileFactory::getMissile(getMissileType(missile)).getDestruction();
//                 m.cost = missilePrice[missile].second; // price
//                 m.safe = (pathInfo.type == "safe");
//                 m.pathType = pathInfo.type;

//                 if (m.safe) {
//                     status.reachedMissiles.push_back(m);
//                 } else {
//                     exposed.push_back(m);
//                 }
//             }
//         }
//     }

//     // Sort exposed missiles by descending damage
//     std::sort(exposed.begin(), exposed.end(), [](const MissileInstance &a, const MissileInstance &b) {
//         return a.damage > b.damage;
//     });

//     // Apply target defense
//     int destroyCount = target.defenseLevel;
//     for (auto &m : exposed) {
//         if (destroyCount > 0) {
//             --destroyCount; // missile destroyed
//         } else {
//             status.reachedMissiles.push_back(m); // missile survives
//         }
//     }

//     targetMissilesMap[target.name] = status;
// }
