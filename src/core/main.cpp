#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <map>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include "../../include/core/city.hpp"
#include "../../include/core/graph.hpp"
#include "../../include/core/input_handler.hpp"

// Define exception to stop search when goal is found
struct goal_found : public std::exception {
    const char* what() const noexcept override { return "Goal found"; }
};

// Custom A* visitor to throw exception on goal
struct astar_goal_visitor : public boost::default_astar_visitor {
    Graph::VertexDescriptor goal;
    explicit astar_goal_visitor(Graph::VertexDescriptor goal) : goal(goal) {}

    template <typename VertexT, typename GraphT>
    void examine_vertex(VertexT u, const GraphT&) {
        if (u == goal) throw goal_found();
    }
};

// Heuristic: use Euclidean distance between coordinates
class city_heuristic : public boost::astar_heuristic<Graph::GraphType, double> {
    Graph::GraphType& graph;
    Graph::VertexDescriptor goal;

public:
    city_heuristic(Graph::GraphType& g, Graph::VertexDescriptor g_goal)
        : graph(g), goal(g_goal) {}

    double operator()(Graph::VertexDescriptor v) {
        const auto& city1 = graph[v];
        const auto& city2 = graph[goal];

        double dx = city1->getLatitude() - city2->getLatitude();
        double dy = city1->getLongitude() - city2->getLongitude();
        return std::sqrt(dx * dx + dy * dy);
    }
};

// Run A* search between city names
void runAStar(Graph& g, const std::string& startCity, const std::string& goalCity) {
    auto graphData = g.getCitiesGraph();
    auto cityMap = g.getCitiesVertex();

    if (cityMap.count(startCity) == 0 || cityMap.count(goalCity) == 0) {
        std::cerr << "Start or goal city not found.\n";
        return;
    }

    auto start = cityMap[startCity];
    auto goal = cityMap[goalCity];

    std::vector<Graph::VertexDescriptor> predecessors(boost::num_vertices(graphData));
    std::vector<double> distances(boost::num_vertices(graphData), std::numeric_limits<double>::max());

    try {
        boost::astar_search(
            graphData,
            start,
            city_heuristic(graphData, goal),
            boost::predecessor_map(&predecessors[0])
                .distance_map(&distances[0])
                .visitor(astar_goal_visitor(goal))
        );
    } catch (const goal_found&) {
        std::cout << "A* path found:\n";

        // Reconstruct path from goal to start
        std::vector<Graph::VertexDescriptor> path;
        for (auto v = goal; v != start; v = predecessors[v])
            path.push_back(v);
        path.push_back(start);
        std::reverse(path.begin(), path.end());

        for (auto v : path) {
            std::cout << " -> " << graphData[v]->getName();
        }
        std::cout << "\nTotal distance: " << distances[goal] << "\n";
        return;
    }

    std::cout << "No path found.\n";
}

void runDijkstra(Graph& g, const std::string& startCity, const std::string& goalCity) {
    auto graphData = g.getCitiesGraph();
    auto cityMap = g.getCitiesVertex();

    if (cityMap.count(startCity) == 0 || cityMap.count(goalCity) == 0) {
        std::cerr << "Start or goal city not found.\n";
        return;
    }

    auto start = cityMap[startCity];
    auto goal = cityMap[goalCity];

    std::vector<Graph::VertexDescriptor> predecessors(boost::num_vertices(graphData));
    std::vector<double> distances(boost::num_vertices(graphData), std::numeric_limits<double>::max());

    boost::dijkstra_shortest_paths(
        graphData,
        start,
        boost::predecessor_map(&predecessors[0])
              .distance_map(&distances[0])
    );

    if (distances[goal] == std::numeric_limits<double>::max()) {
        std::cout << "No path found.\n";
        return;
    }

    std::cout << "Dijkstra shortest path:\n";

    // Reconstruct path
    std::vector<Graph::VertexDescriptor> path;
    for (auto v = goal; v != start; v = predecessors[v]) {
        path.push_back(v);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    for (auto v : path) {
        std::cout << " -> " << graphData[v]->getName();
    }
    std::cout << "\nTotal distance: " << distances[goal] << "\n";
}

void printAllCityDistances(const Graph& g) {
    auto graphData = g.getCitiesGraph();
    auto cityMap = g.getCitiesVertex();
    std::vector<std::string> cityNames;

    // Create reverse mapping from vertex to city name for output
    std::map<Graph::VertexDescriptor, std::string> vertexToName;
    for (const auto& [name, vertex] : cityMap) {
        vertexToName[vertex] = name;
        cityNames.push_back(name);
    }

    for (const auto& [sourceName, sourceVertex] : cityMap) {
        std::vector<double> distances(boost::num_vertices(graphData), std::numeric_limits<double>::max());

        boost::dijkstra_shortest_paths(
            graphData,
            sourceVertex,
            boost::distance_map(&distances[0])
        );

        std::cout << "\nDistances from " << sourceName << ":\n";

        for (const auto& [targetName, targetVertex] : cityMap) {
            if (distances[targetVertex] == std::numeric_limits<double>::max()) {
                std::cout << "  to " << targetName << ": unreachable\n";
            } else {
                std::cout << "  to " << targetName << ": " << distances[targetVertex] << "\n";
            }
        }
    }
}


void printDirectCityDistances(const Graph& g) {
    auto graphData = g.getCitiesGraph();
    auto cityMap = g.getCitiesVertex();

    // Create reverse mapping: vertex → city name
    std::map<Graph::VertexDescriptor, std::string> vertexToName;
    for (const auto& [name, vertex] : cityMap) {
        vertexToName[vertex] = name;
    }

    std::cout << "\n===== Direct Distances Between Cities =====\n";

    // Iterate through all edges and print their weights
    auto edges = boost::edges(graphData);
    for (auto it = edges.first; it != edges.second; ++it) {
        auto src = boost::source(*it, graphData);
        auto tgt = boost::target(*it, graphData);

        double weight = boost::get(boost::edge_weight, graphData, *it);

        std::cout << vertexToName[src] << " <--> " << vertexToName[tgt]
                  << " = " << weight << "\n";
    }
}


struct NoSpyFilter {
    const Graph::GraphType* g;

    NoSpyFilter() = default;
    NoSpyFilter(const Graph::GraphType& graph) : g(&graph) {}

    bool operator()(const Graph::VertexDescriptor& v) const {
        auto city = (*g)[v];
        return city && !city->hasSpy(); // allow if not a spy city
    }

    bool operator()(const Graph::EdgeDescriptor&) const {
        return true; // no edge filtering
    }
};

void printShortestPathIgnoringSpies(const Graph& g, const std::string& srcName, const std::string& destName) {
    auto graphData = g.getCitiesGraph();
    auto cityMap = g.getCitiesVertex();

    auto itSrc = cityMap.find(srcName);
    auto itDest = cityMap.find(destName);

    if (itSrc == cityMap.end() || itDest == cityMap.end()) {
        std::cerr << "Source or destination not found\n";
        return;
    }

    auto src = itSrc->second;
    auto dest = itDest->second;

    NoSpyFilter filter(graphData);
    auto filtered = boost::make_filtered_graph(graphData, filter, filter);

    std::vector<Graph::VertexDescriptor> predecessors(boost::num_vertices(graphData));
    std::vector<double> distances(boost::num_vertices(graphData), std::numeric_limits<double>::max());

    boost::dijkstra_shortest_paths(
        filtered,
        src,
        boost::predecessor_map(&predecessors[0]).distance_map(&distances[0])
    );

    if (distances[dest] == std::numeric_limits<double>::max()) {
        std::cout << "No path found (might be blocked by spy cities).\n";
        return;
    }

    // Reconstruct path
    std::vector<Graph::VertexDescriptor> path;
    for (auto v = dest; v != src; v = predecessors[v]) {
        path.push_back(v);
    }
    path.push_back(src);
    std::reverse(path.begin(), path.end());

    std::cout << "Path from " << srcName << " to " << destName << " (avoiding spies):\n";
    for (auto v : path) {
        std::cout << g.getCitiesGraph()[v]->getName() << " -> ";
    }
    std::cout << "END\nTotal distance: " << distances[dest] << "\n";
}

int main() {
    Graph g;
    InputHandler::loadFromFile(g);

    // runAStar(g, "Zarnovia", "Xenvar");
    // runDijkstra(g, "Zarnovia", "Xenvar");
    // printAllCityDistances(g);
    // printAllCityDistances(g);
    printShortestPathIgnoringSpies(g, "Zarnovia", "Xenvar");
    return 0;
}
