#include "database/database.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>

// Phase 3: file-backed JSON persistence so the project builds without
// requiring system libsqlite3-dev. A full SQLite amalgamation can be
// dropped in later without changing the public API.

namespace recongraph::db {

namespace fs = std::filesystem;

static std::string now_iso() {
    auto t = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(t);
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm, &tt);
#else
    gmtime_r(&tt, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

Database::Database(const std::string& path) : path_(path) {}

Database::~Database() { close(); }

bool Database::open() {
    try {
        fs::path p(path_);
        if (p.has_parent_path()) {
            fs::create_directories(p.parent_path());
        }
        // Touch the file
        std::ofstream f(path_, std::ios::app);
        open_ = f.good();
        return open_;
    } catch (...) {
        open_ = false;
        return false;
    }
}

void Database::close() {
    open_ = false;
}

bool Database::initialize_schema() {
    // For the file-backed approach we keep a simple sessions index file.
    // Real SQLite schema is in database/schema/001_initial.sql for reference.
    return open();
}

bool Database::exec(const std::string&) {
    return true;
}

std::int64_t Database::create_session(const std::string& target_url, int max_depth, int max_requests) {
    if (!open_) return -1;

    // Simple auto-increment via counting existing session files
    std::int64_t id = 1;
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            auto name = entry.path().filename().string();
            if (name.rfind("session_", 0) == 0 && name.size() > 8) {
                try {
                    std::int64_t n = std::stoll(name.substr(8));
                    if (n >= id) id = n + 1;
                } catch (...) {}
            }
        }
    } catch (...) {}

    std::ofstream f("session_" + std::to_string(id) + ".json");
    if (!f) return -1;

    f << "{\n"
      << "  \"id\": " << id << ",\n"
      << "  \"target_url\": \"" << target_url << "\",\n"
      << "  \"max_depth\": " << max_depth << ",\n"
      << "  \"max_requests\": " << max_requests << ",\n"
      << "  \"status\": \"running\",\n"
      << "  \"started_at\": \"" << now_iso() << "\",\n"
      << "  \"finished_at\": null,\n"
      << "  \"stats\": {},\n"
      << "  \"graph\": null\n"
      << "}\n";

    return id;
}

bool Database::update_session_status(std::int64_t session_id, const std::string& status) {
    // Minimal implementation for Phase 3
    (void)session_id;
    (void)status;
    return true;
}

bool Database::finish_session(std::int64_t session_id, const crawler::ScanStats& stats) {
    std::string path = "session_" + std::to_string(session_id) + ".json";
    std::ifstream in(path);
    if (!in) return false;

    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    // Very lightweight rewrite of status + stats
    auto replace_field = [&](const std::string& key, const std::string& value) {
        auto pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return;
        auto colon = content.find(':', pos);
        if (colon == std::string::npos) return;
        auto start = content.find_first_not_of(" \t\n\r", colon + 1);
        if (start == std::string::npos) return;
        auto end = content.find_first_of(",}\n", start);
        content.replace(start, end - start, value);
    };

    replace_field("status", "\"completed\"");
    replace_field("finished_at", "\"" + now_iso() + "\"");

    std::ostringstream stats_json;
    stats_json << "{\"pages_visited\":" << stats.pages_visited
               << ",\"links_discovered\":" << stats.links_discovered
               << ",\"forms_found\":" << stats.forms_found
               << ",\"parameters_found\":" << stats.parameters_found
               << ",\"duplicates_skipped\":" << stats.duplicates_skipped
               << ",\"errors\":" << stats.errors << "}";

    auto stats_pos = content.find("\"stats\"");
    if (stats_pos != std::string::npos) {
        auto colon = content.find(':', stats_pos);
        auto start = content.find_first_not_of(" \t\n\r", colon + 1);
        auto end = content.find_first_of(",}\n", start);
        // Handle object
        if (content[start] == '{') {
            int depth = 0;
            end = start;
            do {
                if (content[end] == '{') ++depth;
                else if (content[end] == '}') --depth;
                ++end;
            } while (depth > 0 && end < content.size());
        }
        content.replace(start, end - start, stats_json.str());
    }

    std::ofstream out(path);
    out << content;
    return true;
}

std::optional<SessionRecord> Database::get_session(std::int64_t session_id) {
    std::string path = "session_" + std::to_string(session_id) + ".json";
    std::ifstream in(path);
    if (!in) return std::nullopt;

    SessionRecord rec;
    rec.id = session_id;
    // Minimal parse for Phase 3 – full JSON parser comes later
    rec.status = "completed";
    return rec;
}

std::vector<SessionRecord> Database::list_sessions(int limit) {
    std::vector<SessionRecord> result;
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            auto name = entry.path().filename().string();
            if (name.rfind("session_", 0) == 0) {
                try {
                    std::int64_t id = std::stoll(name.substr(8));
                    SessionRecord r;
                    r.id = id;
                    r.status = "completed";
                    result.push_back(r);
                    if (static_cast<int>(result.size()) >= limit) break;
                } catch (...) {}
            }
        }
    } catch (...) {}
    return result;
}

bool Database::save_graph(std::int64_t session_id, const graph::AttackSurfaceGraph& g) {
    std::string path = "session_" + std::to_string(session_id) + "_graph.json";
    std::ofstream out(path);
    if (!out) return false;
    out << g.to_json_summary();
    return true;
}

bool Database::load_graph(std::int64_t session_id, graph::AttackSurfaceGraph& /*out*/) {
    // Full reload will be implemented when we add a proper JSON parser
    (void)session_id;
    return false;
}

bool Database::log_event(std::int64_t session_id, const std::string& type, const std::string& message) {
    std::ofstream out("session_" + std::to_string(session_id) + "_events.log", std::ios::app);
    if (!out) return false;
    out << now_iso() << " [" << type << "] " << message << "\n";
    return true;
}

} // namespace recongraph::db
