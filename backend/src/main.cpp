/**
 * ReconGraph - Attack-Surface Intelligence Engine
 * Phase 6 complete – polished foundation ready for portfolio use
 */

#include "api/server.hpp"

#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    int port = 8080;

    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Invalid port. Using default 8080." << std::endl;
        }
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  ReconGraph Backend v0.6.0" << std::endl;
    std::cout << "  Attack-Surface Intelligence Engine" << std::endl;
    std::cout << "  Phase 6 – Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    recongraph::api::Server server(port);
    server.start();

    return 0;
}
