#include "api/server.hpp"

#include <httplib.h>
#include <iostream>
#include <chrono>
#include <ctime>

namespace recongraph::api {

Server::Server(int port) : port_(port) {}

void Server::setup_routes() {
    // Routes are registered inside start() for simplicity in Phase 1
}

void Server::start() {
    httplib::Server svr;

    // Health / status endpoint
    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({
  "status": "ok",
  "service": "ReconGraph",
  "version": "0.1.0",
  "phase": "1 - Foundation",
  "message": "Backend is running"
})",
            "application/json"
        );
    });

    // Simple root info
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({
  "name": "ReconGraph",
  "description": "Attack-Surface Intelligence & Reconnaissance Engine",
  "api_base": "/api",
  "docs": "See README.md"
})",
            "application/json"
        );
    });

    // CORS-friendly headers for development
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    std::cout << "[ReconGraph] Starting server on http://localhost:" << port_ << std::endl;
    std::cout << "[ReconGraph] Health check: http://localhost:" << port_ << "/api/health" << std::endl;
    std::cout << "[ReconGraph] Press Ctrl+C to stop." << std::endl;

    running_ = true;
    if (!svr.listen("0.0.0.0", port_)) {
        std::cerr << "[ReconGraph] Failed to start server on port " << port_ << std::endl;
        running_ = false;
    }
}

void Server::stop() {
    running_ = false;
    // cpp-httplib stop is handled via signal in full implementation
}

} // namespace recongraph::api
