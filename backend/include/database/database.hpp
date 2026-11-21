#pragma once

#include "models/node.hpp"
#include "crawler/recon_collector.hpp"
#include "graph/attack_surface_graph.hpp"

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace recongraph::db {

struct SessionRecord {
    std::int64_t id = 0;
    std::string target_url;
    int max_depth = 3;
    int max_requests = 100;
    std::string status;
    std::string started_at;
    std::string finished_at;
    crawler::ScanStats stats;
};

/**
 * Lightweight SQLite persistence layer for Phase 3.
 * Uses the amalgamated sqlite3 if available, otherwise falls back to
 * a simple in-memory + JSON file approach so the project still builds
 * without system SQLite development packages.
 */
class Database {
public:
    explicit Database(const std::string& path = "recongraph.db");
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool open();
    void close();
    bool is_open() const { return open_; }

    // Schema
    bool initialize_schema();

    // Sessions
    std::int64_t create_session(const std::string& target_url, int max_depth, int max_requests);
    bool update_session_status(std::int64_t session_id, const std::string& status);
    bool finish_session(std::int64_t session_id, const crawler::ScanStats& stats);
    std::optional<SessionRecord> get_session(std::int64_t session_id);
    std::vector<SessionRecord> list_sessions(int limit = 50);

    // Graph persistence
    bool save_graph(std::int64_t session_id, const graph::AttackSurfaceGraph& g);
    bool load_graph(std::int64_t session_id, graph::AttackSurfaceGraph& out);

    // Events
    bool log_event(std::int64_t session_id, const std::string& type, const std::string& message);

private:
    std::string path_;
    bool open_ = false;
    void* db_handle_ = nullptr; // opaque (sqlite3* when available)

    bool exec(const std::string& sql);
};

} // namespace recongraph::db
