#include "scenario5.hpp"

#include <iostream>
#include <queue>

#include "base_city.hpp"
#include "missile_factory.hpp"
#include "target_city.hpp"
#include "scenario5_input.hpp"

Scenario5::Scenario5()
    : nights(5, false) {}

void Scenario5::solve()
{
  Scenario5Input::fillInventory(std::cin, inventory);
  Scenario5Input::createCities(std::cin, Scenario::mapInformation);
  initialize();
  findPaths();

  for (int i{}; i < 5; ++i)
  {
    buildMissilePathMap();
    // logMissilePaths();
    attack(i);
    if (i != 4)
      Scenario5Input::updateSpies(std::cin, Scenario::mapInformation);
  }

  for (auto n : nights)
    std::cout << (n ? "✅" : "❌") << " ";
}

void Scenario5::initialize()
{
  // uncontrolledDistance is set to max between all missiles
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

void Scenario5::findPaths()
{
  const auto &citiesGraph = Scenario::mapInformation.getCitiesGraph();
  // std::vector<Graph::VertexDescriptor> baseVertices;
  // std::vector<Graph::VertexDescriptor> targetVertices;

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

void Scenario5::buildMissilePathMap()
{
  missilePathMap.clear(); // Clear existing entries

  // Define all missile types with their identifiers
  static const std::vector<std::pair<std::string, Missile>> missileList = {
      {"A1", MissileFactory::A1}, {"A2", MissileFactory::A2}, {"A3", MissileFactory::A3}, {"B1", MissileFactory::B1}, {"B2", MissileFactory::B2}, {"C1", MissileFactory::C1}, {"C2", MissileFactory::C2}, {"D1", MissileFactory::D1}};

  for (auto &path : paths)
  {
    // Update spy count for current path
    path.updatePathSpies(*this);

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
}

void Scenario5::logMissilePaths(std::string logType)
{
  auto graph = mapInformation.getCitiesGraph();

  if (logType == "verbose")
  {
    for (const auto &[category, pathList] : missilePathMap)
    {
      std::cout << "\n===== " << category << " ===== (" << pathList.size()
                << " paths)\n";

      for (const auto &path : pathList)
      {
        std::string baseName = graph[path.base]->getName();
        std::string targetName = graph[path.target]->getName();

        std::cout << "\nPath: " << baseName << " -> " << targetName
                  << " | Distance: " << path.distance
                  << " | Max Gap: " << path.max_gap
                  << " | Spies: " << path.spyCount << "\n";

        std::cout << "Cities: ";
        for (const auto &city : path.cities)
        {
          std::cout << city;
          // Check if city has spy
          auto it = mapInformation.getCitiesVertex().find(city);
          if (it != mapInformation.getCitiesVertex().end())
          {
            auto vertex = it->second;
            if (graph[vertex]->hasSpy())
            {
              std::cout << "(spy) ";
            }
            else
            {
              std::cout << " ";
            }
          }
        }
        std::cout << "\n";
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

// we will first check for weakest missiles safe paths
// checking safe paths for these missiles, if found instantly shoot and mark
// night as done
void Scenario5::attack(int nightIdx)
{
  std::cout << "\n**** Starting safe attack phase *****\n";
  auto &graph = mapInformation.getCitiesGraphRef();

  // missile priority order
  std::vector<std::string> missilePriority = {"B2", "C2", "D1", "A2",
                                              "B1", "C1", "A1", "A3"};

  for (const auto &mtypeStr : missilePriority)
  {
    int &count = [&]() -> int &
    {
      if (mtypeStr == "A1")
        return inventory.A1;
      if (mtypeStr == "A2")
        return inventory.A2;
      if (mtypeStr == "A3")
        return inventory.A3;
      if (mtypeStr == "B1")
        return inventory.B1;
      if (mtypeStr == "B2")
        return inventory.B2;
      if (mtypeStr == "C1")
        return inventory.C1;
      if (mtypeStr == "C2")
        return inventory.C2;
      if (mtypeStr == "D1")
        return inventory.D1;
      static int dummy = 0;
      return dummy;
    }();

    // Skip if no missiles of this type available
    if (count <= 0)
      continue;

    // get missile information
    auto mt = getMissileType(mtypeStr);
    auto missile = MissileFactory::getMissile(mt);
    int damagePerMissile = missile.getDestruction();
    auto &missilePaths = missilePathMap[mtypeStr + " safe"];

    // find the first valid path for this missile type
    for (auto it = missilePaths.begin(); it != missilePaths.end(); ++it)
    {
      const auto &baseDesc = it->base;

      // Verify base exists
      auto baseIt =
          std::find(baseVertices.begin(), baseVertices.end(), baseDesc);
      if (baseIt == baseVertices.end())
        continue;

      // Verify base is operational
      auto baseCityPtr = std::dynamic_pointer_cast<BaseCity>(graph[baseDesc]);
      if (!baseCityPtr)
        continue;

      // ATTACK: Shoot exactly 1 missile
      int used = 1;
      int pathDamage = damagePerMissile * used;
      count -= used;

      // Print attack details
      std::cout << "Shot " << used << "x " << mtypeStr << " missile via path: ";
      for (const auto &city : it->cities)
        std::cout << city << " ";
      std::cout << "| Damage: " << pathDamage << "\n";

      nights[nightIdx] = true;
      return; // return after first successful attack, we are done for this
              // night
    }
  }
  std::cout << "No valid safe paths found for any missile type\n";

  if (!nights[nightIdx])
  {
    std::cout << "\n**** Starting fallback attack phase *****\n";
    bool success = attackFallbackPhase();
    nights[nightIdx] = success;
  }
}

bool Scenario5::attackFallbackPhase()
{
  // Find the most efficient target (least total missiles needed to get 1 hit)
  auto bestOption = findMostEfficientTarget();

  if (bestOption.totalMissilesNeeded == 0)
  {
    std::cout << "\n**** No viable targets found for fallback phase ****\n";
    return false;
  }

  if (bestOption.totalMissilesNeeded == INT_MAX)
  {
    std::cout << "\nfallback attack failed\n\n";
    return false;
  }

  std::cout << "\n**** Fallback Phase: Targeting '" << bestOption.targetName
            << "' (needs " << bestOption.totalMissilesNeeded
            << " missiles to guarantee 1 hit) ****\n";
  // Execute the minimal attack

  return executeMinimalAttack(bestOption);
}

// === Find target that requires fewest missiles to guarantee 1 hit ===
Scenario5::MinimalTarget Scenario5::findMostEfficientTarget()
{
  MinimalTarget best;
  best.totalMissilesNeeded = INT_MAX;

  auto fallbackBases = collectFallbackBases();
  if (fallbackBases.empty())
    return best;

  auto &graph = mapInformation.getCitiesGraphRef();
  auto cities = mapInformation.getCitiesVertex();

  // Check each potential target
  for (auto &[cityName, vertex] : cities)
  {
    auto targetPtr = std::dynamic_pointer_cast<TargetCity>(graph[vertex]);
    if (!targetPtr)
      continue;

    int defenseLevel = targetPtr->getDefenseLevel();
    int missilesNeeded = defenseLevel + 1; // need defense+1 to guarantee 1 hit

    // Find if we can actually fire this many missiles at this target
    auto attackPlan =
        planMinimalAttack(fallbackBases, cityName, missilesNeeded);
    if (attackPlan.empty())
      continue; // can't reach this target with enough missiles

    // Check if this is more efficient than current best
    if (missilesNeeded < best.totalMissilesNeeded)
    {
      best.targetName = cityName;
      best.defenseLevel = defenseLevel;
      best.totalMissilesNeeded = missilesNeeded;
      best.attackPlan = std::move(attackPlan);
    }
  }
  // TODO: handle the situation better if best totalMissilesNeeded == INT_MAX
  return best;
}

// === Plan how to fire exact number of missiles at target ===
std::vector<Scenario5::AttackStep> Scenario5::planMinimalAttack(
    const std::vector<FallbackBase> &bases, const std::string &targetName,
    int missilesNeeded)
{
  std::vector<AttackStep> plan;
  int missilesPlanned = 0;
  auto &graph = mapInformation.getCitiesGraphRef();

  // bases are stored in missile priority order by the collectFallbackBases function
  for (const auto &fb : bases)
  {
    if (missilesPlanned >= missilesNeeded)
      break;
    if (fb.inventoryCount == 0)
      continue;

    // Check if this base can reach the target
    bool canHitTarget = false;
    for (const auto &path : fb.paths)
    {
      if (graph[path.target]->getName() == targetName)
      {
        canHitTarget = true;
        break;
      }
    }
    if (!canHitTarget)
      continue;

    // Calculate how many missiles this base can contribute
    auto basePtr = std::dynamic_pointer_cast<BaseCity>(graph[fb.baseDesc]);
    int canFire =
        std::min({fb.inventoryCount, missilesNeeded - missilesPlanned});

    if (canFire > 0)
    {
      plan.push_back({fb.baseDesc, fb.baseName, fb.missileType, canFire,
                      fb.damagePerMissile, getPathToTarget(fb, targetName)});
      missilesPlanned += canFire;
    }
  }

  // Only return plan if we can fire enough missiles
  return (missilesPlanned >= missilesNeeded) ? plan : std::vector<AttackStep>();
}

// === Execute the minimal attack plan ===
bool Scenario5::executeMinimalAttack(const MinimalTarget &target)
{
  auto &graph = mapInformation.getCitiesGraphRef();
  int totalFired = 0;
  int totalBlocked = 0;
  int damageDealt = 0;

  std::cout << "\n*** Executing Minimal Attack on '" << target.targetName
            << "' (defense=" << target.defenseLevel << ") ***\n";

  // Fire missiles according to plan
  for (const auto &step : target.attackPlan)
  {
    std::cout << "Base '" << step.baseName << "' fires " << step.missileCount
              << " x '" << step.missileType
              << "' [dmg=" << step.damagePerMissile << "] via path:";
    for (const auto &city : step.pathCities)
    {
      std::cout << " " << city;
    }
    std::cout << "\n";

    updateInventory(step.missileType, -step.missileCount);
    totalFired += step.missileCount;
  }

  // Calculate results: first N missiles are blocked, rest hit
  totalBlocked = std::min(totalFired, target.defenseLevel);
  int missilesHit = totalFired - totalBlocked;

  // NOTE: damage is not important part of this scenario so this part is not exactly correct just approximation
  // For damage calculation, use the best missile type that actually hit
  if (missilesHit > 0)
  {
    // Find highest damage missile in our attack plan (first missile to hit)
    int highestDamage = 0;
    for (const auto &step : target.attackPlan)
    {
      highestDamage = std::max(highestDamage, step.damagePerMissile);
    }
    damageDealt = highestDamage; // Only 1 missile hits, use the best one
  }

  std::cout << "Results: Fired=" << totalFired << ", Blocked=" << totalBlocked
            << ", Hit=" << missilesHit << ", Damage=" << damageDealt << "\n";

  bool success = (missilesHit > 0);
  std::cout << "\n**** Fallback mission " << (success ? "SUCCESS" : "FAILED")
            << ": " << damageDealt << " damage ****\n";

  return success;
}

// === Helper: Get path cities for a specific target ===
std::vector<std::string> Scenario5::getPathToTarget(
    const FallbackBase &fb, const std::string &targetName)
{
  auto &graph = mapInformation.getCitiesGraphRef();
  for (const auto &path : fb.paths)
  {
    if (graph[path.target]->getName() == targetName)
    {
      return path.cities;
    }
  }
  return {};
}

// === Helper: Update inventory counts ===
void Scenario5::updateInventory(const std::string &missileType, int change)
{
  if (missileType == "A1")
    inventory.A1 += change;
  else if (missileType == "A2")
    inventory.A2 += change;
  else if (missileType == "A3")
    inventory.A3 += change;
  else if (missileType == "B1")
    inventory.B1 += change;
  else if (missileType == "B2")
    inventory.B2 += change;
  else if (missileType == "C1")
    inventory.C1 += change;
  else if (missileType == "C2")
    inventory.C2 += change;
  else if (missileType == "D1")
    inventory.D1 += change;
}

std::vector<Scenario5::FallbackBase> Scenario5::collectFallbackBases()
{
  std::vector<FallbackBase> bases;
  const std::vector<std::string> missileOrder = {"B2", "C2", "D1", "A2",
                                                 "B1", "C1", "A1", "A3"};

  for (const auto &type : missileOrder)
  {
    int count = getInventoryCount(type);
    if (count <= 0)
      continue;

    auto missile = MissileFactory::getMissile(getMissileType(type));
    int damage = missile.getDestruction();
    auto allPaths = missilePathMap[type + " revealed"];

    for (auto baseV : baseVertices)
    {
      std::vector<PathInfo> valid;
      for (auto &p : allPaths)
      {
        if (p.base == baseV)
          valid.push_back(p);
      }
      if (valid.empty())
        continue;

      auto basePtr = std::dynamic_pointer_cast<BaseCity>(
          mapInformation.getCitiesGraphRef()[baseV]);
      if (!basePtr)
        continue;
      else
      {
        bases.push_back(
            {baseV, basePtr->getName(), type, count, damage, std::move(valid)});
        // break early so we have only one base for each missile type
        break;
      }
    }
  }
  return bases;
}

int Scenario5::getInventoryCount(const std::string &type)
{
  if (type == "A1")
    return inventory.A1;
  if (type == "A2")
    return inventory.A2;
  if (type == "A3")
    return inventory.A3;
  if (type == "B1")
    return inventory.B1;
  if (type == "B2")
    return inventory.B2;
  if (type == "C1")
    return inventory.C1;
  if (type == "C2")
    return inventory.C2;
  if (type == "D1")
    return inventory.D1;
  return 0;
}