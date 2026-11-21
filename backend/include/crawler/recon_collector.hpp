#pragma once

#include "graph/attack_surface_graph.hpp"
#include "core/url_normalizer.hpp"
#include "models/node.hpp"

#include <string>
#include <vector>
#include <functional>
#include <atomic>

namespace recongraph::crawler {

struct ScanConfig {
    std::string target_url;
    std::vector<std::string> allowed_domains;  // empty = only target host
    int max_depth = 3;
    int max_requests = 100;
    int request_timeout_ms = 5000;
    bool follow_external = false;
    bool respect_robots = true;               // placeholder for later
};

struct ScanStats {
    int pages_visited = 0;
    int links_discovered = 0;
    int forms_found = 0;
    int parameters_found = 0;
    int duplicates_skipped = 0;
    int errors = 0;
};

/**
 * Controlled Reconnaissance Collector
 *
 * - Bounded by depth and request limits
 * - Uses custom Queue for BFS crawling
 * - Uses custom HashTable (via graph) for deduplication
 * - Only follows links within allowed scope
 * - Does NOT perform brute-force, credential attacks, or destructive actions
 */
class ReconCollector {
public:
    using ProgressCallback = std::function<void(const std::string& message, int progress_pct)>;

    explicit ReconCollector(const ScanConfig& config);

    // Run a controlled scan. Returns the populated attack-surface graph.
    graph::AttackSurfaceGraph run(ProgressCallback on_progress = nullptr);

    // Cancel an in-progress scan
    void cancel();

    const ScanStats& stats() const { return stats_; }
    bool was_cancelled() const { return cancelled_.load(); }

private:
    ScanConfig config_;
    ScanStats stats_;
    std::atomic<bool> cancelled_{false};

    bool is_in_scope(const std::string& url) const;
    models::NodeType classify_url(const std::string& path) const;

    // Lightweight HTML link/form extraction (regex-based for Phase 2)
    std::vector<std::string> extract_links(const std::string& html, const std::string& base_url);
    std::vector<std::pair<std::string, std::vector<std::string>>> extract_forms(const std::string& html);
};

} // namespace recongraph::crawler
