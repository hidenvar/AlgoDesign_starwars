#include <iostream>
#include <queue>
#include <sstream>
#include <iomanip>

#include <boost/graph/adjacency_list.hpp>

#include "scenario1.hpp"
#include "base_city.hpp"
#include "target_city.hpp"

#define CLR_RESET "\033[0m"
#define CLR_GREEN "\033[32m"
#define CLR_RED "\033[31m"
#define CLR_YELLOW "\033[33m"
#define CLR_CYAN "\033[36m"
#define CLR_BOLD "\033[1m"

Scenario1::Scenario1(Graph &g) : Scenario(g) {}

// create a graph that connects two cities if their distance is less than uncontrolledDistance
void Scenario1::initialize()
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

// performs bfs on the graph to find all paths from base to target
void Scenario1::findPaths()
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

      // Check if reached target
      bool isTarget = (std::find(targetVertices.begin(), targetVertices.end(), currentVertex) != targetVertices.end());
      if (isTarget)
      {
        // Convert path to city names and count spies
        PathInfo pathInfo;
        pathInfo.base = base;
        pathInfo.target = currentVertex;
        int spyCount = 0;

        for (auto vd : currentPath)
        {
          const auto &city = citiesGraph[vd];
          pathInfo.cities.push_back(city->getName());
          if (city->hasSpy())
          {
            spyCount++;
          }
        }

        pathInfo.spyCount = spyCount;
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

void Scenario1::buildBaseToPathsMap()
{
  baseToPathsMap.clear();
  auto compareSpyCount = [](const PathInfo &a, const PathInfo &b)
  { return a.spyCount < b.spyCount; };
  for (const auto &path : paths)
  {
    baseToPathsMap[path.base].push_back(path);
    sort(baseToPathsMap[path.base].begin(), baseToPathsMap[path.base].end(), compareSpyCount);
  }
}

const std::vector<Scenario1::PathInfo> &Scenario1::getPathsFromBase(Graph::VertexDescriptor base) const
{
  static const std::vector<PathInfo> empty; // empty vector for missing keys
  auto it = baseToPathsMap.find(base);
  return (it != baseToPathsMap.end()) ? it->second : empty;
}

void Scenario1::attack()
{
  int totalDamage = 0;
  auto cities = Scenario1::mapInformation.getCitiesGraph();
  auto nameToDesc = mapInformation.getCitiesVertex();

  struct FallbackBase
  {
    Graph::VertexDescriptor base;
    std::string baseName;
    std::unordered_map<std::string, int> missileInventory;
    int damagePerMissile;
    std::vector<PathInfo> paths;
  };

  std::vector<FallbackBase> fallbackBases;

  std::cout << CLR_BOLD << "=== PHASE 1: SAFE ATTACKS ===" << CLR_RESET << "\n";

  for (const auto base : baseVertices)
  {
    auto cityPtr = cities[base];
    auto baseCityPtr = std::dynamic_pointer_cast<BaseCity>(cityPtr);
    const auto &missiles = baseCityPtr->getMissiles();
    const auto &paths = getPathsFromBase(base);

    bool hasSafe = false;
    for (const auto &m : missiles)
    {
      if (!paths.empty() && paths[0].spyCount < m.first.getStealth())
      {
        hasSafe = true;

        std::cout << CLR_GREEN << "✔ Base " << baseCityPtr->getName()
                  << " fires " << m.second << " x " << m.first.getType()
                  << " via safe path: " << CLR_RESET;

        for (const auto &name : paths[0].cities)
          std::cout << name << " ";
        std::cout << "\n";

        int dmg = m.second * m.first.getDestruction();
        totalDamage += dmg;
        std::cout << "   Damage dealt: " << CLR_RED << dmg << CLR_RESET << "\n\n";
      }
    }

    if (!hasSafe && !paths.empty())
    {
      FallbackBase fb;
      fb.base = base;
      fb.baseName = baseCityPtr->getName();
      fb.paths = paths;

      for (const auto &m : missiles)
      {
        std::ostringstream oss;
        oss << m.first.getType();
        std::string typeStr = oss.str();
        fb.missileInventory[typeStr] += m.second;
        fb.damagePerMissile = m.first.getDestruction();
      }

      fallbackBases.push_back(fb);
    }
  }

  const int totalDamagePossible = getTotalDamage();

  std::cout << "\n"
            << CLR_BOLD << "=== PHASE 2: FALLBACK ATTACKS ===" << CLR_RESET << "\n";

  do
  {
    bool missilesLeft = false;
    for (const auto &fb : fallbackBases)
    {
      for (const auto &[_, count] : fb.missileInventory)
      {
        if (count > 0)
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
    std::unordered_map<std::string, int> damagePerMissile;
    std::unordered_map<std::string, std::vector<int>> baseIndexes;

    for (size_t i = 0; i < fallbackBases.size(); ++i)
    {
      const auto &fb = fallbackBases[i];
      for (const auto &path : fb.paths)
      {
        std::string targetName = cities[path.target]->getName();
        int count = 0;
        for (const auto &[type, qty] : fb.missileInventory)
        {
          count += qty;
          damagePerMissile[targetName] = fb.damagePerMissile;
        }
        if (count > 0)
        {
          totalMissilesToTarget[targetName] += count;
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

      Graph::VertexDescriptor tgtDesc = it->second;
      auto tgtCity = std::dynamic_pointer_cast<TargetCity>(cities[tgtDesc]);
      int defense = tgtCity->getDefenseLevel();
      int bypassed = std::max(0, count - defense);
      int damage = bypassed * damagePerMissile[targetName];

      if (damage > maxBypassedDamage)
      {
        maxBypassedDamage = damage;
        bestTarget = targetName;
        bestTargetDefense = defense;
      }
    }

    std::cout << "\n"
              << CLR_CYAN << "*** Rapid-Fire on \"" << bestTarget
              << "\" ***" << CLR_RESET << "\n";
    std::cout << "Defense level: " << bestTargetDefense << "\n";

    int totalFired = 0, totalBlocked = 0;
    std::unordered_map<std::string, int> blockedByType;

    for (int idx : baseIndexes[bestTarget])
    {
      auto &fb = fallbackBases[idx];
      PathInfo *pathToUse = nullptr;
      for (auto &p : fb.paths)
      {
        if (cities[p.target]->getName() == bestTarget)
        {
          pathToUse = &p;
          break;
        }
      }
      if (!pathToUse)
        continue;

      int baseFired = 0;
      std::cout << CLR_YELLOW << "From Base " << fb.baseName << " via path: " << CLR_RESET;
      for (const auto &city : pathToUse->cities)
        std::cout << city << " ";
      std::cout << "\n";

      for (auto &[type, count] : fb.missileInventory)
      {
        if (count == 0)
          continue;
        std::cout << "   - " << count << " x " << type << "\n";
        for (int i = 0; i < count; ++i)
        {
          if (totalBlocked < bestTargetDefense)
          {
            totalBlocked++;
            blockedByType[type]++;
          }
          else
          {
            totalDamage += fb.damagePerMissile;
          }
          totalFired++;
        }
        baseFired += count;
        count = 0;
      }
      if (baseFired == 0)
        std::cout << "   (no missiles left)\n";
    }

    std::cout << CLR_BOLD << "\n--- Attack Summary ---" << CLR_RESET << "\n";
    std::cout << "Total Fired: " << totalFired << "\n";
    std::cout << "Blocked:     " << totalBlocked << "\n";
    std::cout << "Bypassed:    " << (totalFired - totalBlocked) << "\n";
    std::cout << "Blocked by Type:\n";
    for (const auto &[type, cnt] : blockedByType)
      std::cout << "   - " << type << ": " << cnt << "\n";

  } while (totalDamagePossible * 0.9 >= totalDamage);

  std::cout << "\n"
            << CLR_BOLD << "=== FINAL DAMAGE REPORT ===" << CLR_RESET << "\n";
  std::cout << "Total Damage: " << CLR_RED << totalDamage << CLR_RESET << "\n";
}

int Scenario1::getTotalDamage()
{
  int totalDamage = 0;
  const auto &cities = Scenario1::mapInformation.getCitiesGraph();
  for (const auto &base : baseVertices)
  {
    auto cityPtr = cities[base];
    auto baseCityPtr = std::dynamic_pointer_cast<BaseCity>(cityPtr);
    const auto &missiles = baseCityPtr->getMissiles();

    for (const auto &m : missiles)
    {
      totalDamage += m.second * m.first.getDestruction();
    }
  }

  return totalDamage;
}

void Scenario1::solve()
{
  initialize();
  findPaths();
  buildBaseToPathsMap();
  attack();
}

const std::vector<Scenario1::PathInfo> &Scenario1::getPaths() const { return paths; }
const std::vector<Graph::VertexDescriptor> Scenario1::getBaseVertices() const { return baseVertices; }
const std::vector<Graph::VertexDescriptor> Scenario1::getTargetVertices() const { return targetVertices; }