#include "scenario6.hpp"

Scenario6::Scenario6(Graph& g, Inventory& i)
    : Scenario(g), g(g) , in(i) {}

void Scenario6::initialize(){};
void Scenario6::solve() {
    Scenario5Input::fillInventory(std::cin, in);
    Scenario5Input::createCities(std::cin, g);

    // Initialize Scenario5
    Scenario5 s(g, in);
    s.initialize();
    s.findPaths();

    int nightIdx = 0;
    std::vector<bool> results;

    while (true) {
        s.buildMissilePathMap();
        s.logMissilePaths();
        s.nights[nightIdx] = false;
        s.attack(nightIdx);

        bool ok = s.nights[nightIdx];
        results.push_back(ok);

        if (!ok) break;

        Scenario5Input::updateSpies(std::cin, g);
    }

    for (bool r : results) std::cout << (r ? "✅" : "❌") << " ";
    std::cout << "\n";
    std::cout << "*** summary : " << results.size() - 1 << " attacks *** \n" ;
}
