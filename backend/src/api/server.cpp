#include "api/server.hpp"
#include "crawler/recon_collector.hpp"
#include "core/url_normalizer.hpp"
#include "graph/attack_surface_graph.hpp"

#include <httplib.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>

namespace recongraph::api {

// Simple in-memory session store for Phase 2
static std::mutex g_mutex;
static std::unique_ptr<graph::AttackSurfaceGraph> g_last_graph;
static crawler::ScanStats g_last_stats;

Server::Server(int port) : port_(port) {}

void Server::setup_routes() {
    // Implemented inside start()
}

void Server::start() {
    httplib::Server svr;

    // ---------- Health ----------
    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({
  "status": "ok",
  "service": "ReconGraph",
  "version": "0.2.0",
  "phase": "2 - Core Engine",
  "message": "Backend is running with DSA + Graph + Collector"
})",
            "application/json"
        );
    });

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({
  "name": "ReconGraph",
  "description": "Attack-Surface Intelligence & Reconnaissance Engine",
  "api_base": "/api",
  "endpoints": [
    "GET  /api/health",
    "POST /api/normalize",
    "POST /api/scan",
    "GET  /api/graph",
    "GET  /api/stats"
  ]
})",
            "application/json"
        );
    });

    // ---------- URL Normalization (demo / utility) ----------
    svr.Post("/api/normalize", [](const httplib::Request& req, httplib::Response& res) {
        std::string url = req.body;
        // Very lightweight extraction if body is {"url": "..."}
        auto key_pos = url.find("\"url\"");
        if (key_pos != std::string::npos) {
            auto colon = url.find(':', key_pos);
            if (colon != std::string::npos) {
                auto q1 = url.find('"', colon + 1);
                if (q1 != std::string::npos) {
                    auto q2 = url.find('"', q1 + 1);
                    if (q2 != std::string::npos) {
                        url = url.substr(q1 + 1, q2 - q1 - 1);
                    }
                }
            }
        }

        auto norm = core::UrlNormalizer::normalize(url);
        if (!norm) {
            res.status = 400;
            res.set_content(R"({"error": "Invalid URL"})", "application/json");
            return;
        }

        std::ostringstream oss;
        oss << "{\n"
            << "  \"canonical\": \"" << norm->canonical << "\",\n"
            << "  \"scheme\": \"" << norm->scheme << "\",\n"
            << "  \"host\": \"" << norm->host << "\",\n"
            << "  \"port\": " << norm->port << ",\n"
            << "  \"path\": \"" << norm->path << "\",\n"
            << "  \"query\": \"" << norm->query << "\"\n"
            << "}";
        res.set_content(oss.str(), "application/json");
    });

    // ---------- Controlled Scan ----------
    svr.Post("/api/scan", [](const httplib::Request& req, httplib::Response& res) {
        std::string body = req.body;
        std::string target;
        int max_depth = 2;
        int max_requests = 30;

        auto extract_string = [&](const std::string& key) -> std::string {
            std::string pattern = "\"" + key + "\"";
            auto pos = body.find(pattern);
            if (pos == std::string::npos) return {};
            pos = body.find(':', pos);
            if (pos == std::string::npos) return {};
            pos = body.find_first_not_of(" \t\n\r", pos + 1);
            if (pos == std::string::npos) return {};
            if (body[pos] == '"') {
                auto end = body.find('"', pos + 1);
                if (end == std::string::npos) return {};
                return body.substr(pos + 1, end - pos - 1);
            }
            auto end = body.find_first_of(",}", pos);
            return body.substr(pos, end - pos);
        };

        target = extract_string("url");
        try {
            std::string d = extract_string("max_depth");
            if (!d.empty()) max_depth = std::stoi(d);
            std::string r = extract_string("max_requests");
            if (!r.empty()) max_requests = std::stoi(r);
        } catch (...) {}

        if (target.empty()) {
            res.status = 400;
            res.set_content(R"({"error": "Missing 'url' field"})", "application/json");
            return;
        }

        auto norm = core::UrlNormalizer::normalize(target);
        if (!norm) {
            res.status = 400;
            res.set_content(R"({"error": "Invalid target URL"})", "application/json");
            return;
        }

        crawler::ScanConfig cfg;
        cfg.target_url = target;
        cfg.max_depth = std::max(0, std::min(max_depth, 5));
        cfg.max_requests = std::max(1, std::min(max_requests, 100));

        crawler::ReconCollector collector(cfg);
        auto result_graph = collector.run([](const std::string& msg, int pct) {
            std::cout << "[Scan " << pct << "%] " << msg << std::endl;
        });

        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_last_graph = std::make_unique<graph::AttackSurfaceGraph>(std::move(result_graph));
            g_last_stats = collector.stats();
        }

        std::ostringstream oss;
        oss << "{\n"
            << "  \"status\": \"completed\",\n"
            << "  \"target\": \"" << target << "\",\n"
            << "  \"nodes\": " << g_last_graph->node_count() << ",\n"
            << "  \"edges\": " << g_last_graph->edge_count() << ",\n"
            << "  \"pages_visited\": " << g_last_stats.pages_visited << ",\n"
            << "  \"links_discovered\": " << g_last_stats.links_discovered << ",\n"
            << "  \"forms_found\": " << g_last_stats.forms_found << ",\n"
            << "  \"parameters_found\": " << g_last_stats.parameters_found << ",\n"
            << "  \"duplicates_skipped\": " << g_last_stats.duplicates_skipped << ",\n"
            << "  \"errors\": " << g_last_stats.errors << "\n"
            << "}";

        res.set_content(oss.str(), "application/json");
    });

    // ---------- Get last graph ----------
    svr.Get("/api/graph", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_last_graph) {
            res.set_content(R"({"node_count": 0, "edge_count": 0, "nodes": []})", "application/json");
            return;
        }
        res.set_content(g_last_graph->to_json_summary(), "application/json");
    });

    // ---------- Stats ----------
    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::ostringstream oss;
        oss << "{\n"
            << "  \"pages_visited\": " << g_last_stats.pages_visited << ",\n"
            << "  \"links_discovered\": " << g_last_stats.links_discovered << ",\n"
            << "  \"forms_found\": " << g_last_stats.forms_found << ",\n"
            << "  \"parameters_found\": " << g_last_stats.parameters_found << ",\n"
            << "  \"duplicates_skipped\": " << g_last_stats.duplicates_skipped << ",\n"
            << "  \"errors\": " << g_last_stats.errors << "\n"
            << "}";
        res.set_content(oss.str(), "application/json");
    });

    // CORS
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    std::cout << "[ReconGraph] Starting server on http://localhost:" << port_ << std::endl;
    std::cout << "[ReconGraph] Phase 2 – Core Engine ready" << std::endl;
    std::cout << "[ReconGraph] Health : http://localhost:" << port_ << "/api/health" << std::endl;
    std::cout << "[ReconGraph] Scan   : POST /api/scan" << std::endl;
    std::cout << "[ReconGraph] Graph  : GET  /api/graph" << std::endl;

    running_ = true;
    if (!svr.listen("0.0.0.0", port_)) {
        std::cerr << "[ReconGraph] Failed to start server on port " << port_ << std::endl;
        running_ = false;
    }
}

void Server::stop() {
    running_ = false;
}

} // namespace recongraph::api
