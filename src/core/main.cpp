#include <iostream>
#include "graph.hpp"
#include "input_handler.hpp"
#include "base_city.hpp"
#include "target_city.hpp"
#include "scenario1.hpp"


// void printAllCities(const Graph& graph) {
//     auto graphData = graph.getCitiesGraph();
//     auto cityMap = graph.getCitiesVertex();

//     // Build reverse map: VertexDescriptor → city name
//     std::map<Graph::VertexDescriptor, std::string> vertexToName;
//     for (const auto& [name, vd] : cityMap) {
//         vertexToName[vd] = name;
//     }

//     auto [vi, vi_end] = boost::vertices(graphData);
//     for (; vi != vi_end; ++vi) {
//         auto cityPtr = graphData[*vi];
//         std::string cityName = vertexToName[*vi];

//         std::cout << "---------------------------\n";
//         std::cout << "Name: " << cityPtr->getName() << "\n";
//         std::cout << "Country: " << cityPtr->getCountry() << "\n";
//         std::cout << "Latitude: " << cityPtr->getLatitude() << "\n";
//         std::cout << "Longitude: " << cityPtr->getLongitude() << "\n";
//         std::cout << "Strategic Points: " << cityPtr->hasSpy() << "\n";

//         switch (cityPtr->getType()) {
//             case CityType::NORMAL:
//                 std::cout << "Type: Normal City\n";
//                 break;
//             case CityType::BASE: {
//                 std::cout << "Type: Base City\n";
//                 auto baseCity = std::dynamic_pointer_cast<BaseCity>(cityPtr);
//                 if (baseCity) {
//                     std::cout << "Missiles:\n";
//                     for (const auto& [missile, count] : baseCity->getMissiles()) {
//                         std::cout << "  - " << missile.getName() << " x" << count << "\n";
//                     }
//                 }
//                 break;
//             }
//             case CityType::TARGET: {
//                 std::cout << "Type: Target City\n";
//                 auto targetCity = std::dynamic_pointer_cast<TargetCity>(cityPtr);
//                 if (targetCity) {
//                     std::cout << "Defense Level: " << targetCity->getDefenseLevel() << "\n";
//                 }
//                 break;
//             }
//         }

//         // Show connected cities (neighbors)
//         std::cout << "Connected to: ";
//         bool first = true;
//         auto [ei, ei_end] = boost::out_edges(*vi, graphData);
//         for (; ei != ei_end; ++ei) {
//             auto target = boost::target(*ei, graphData);
//             if (!first) std::cout << ", ";
//             std::cout << vertexToName[target];
//             first = false;
//         }
//         if (first) std::cout << "None"; // No neighbors
//         std::cout << "\n";

//         std::cout << "---------------------------\n\n";
//     }
// }


int main(int argc, char* argv[]) {
    if (argc < 2) return 0;
    Graph g;
    InputHandler::loadFromFile(g);

    int scenario = std::stoi(argv[1]);
    Scenario1 s(g);
    s.solve();
}