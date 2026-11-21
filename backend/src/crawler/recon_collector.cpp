#include "crawler/recon_collector.hpp"
#include "dsa/data_structures/queue/queue.hpp"

#include <httplib.h>
#include <regex>
#include <iostream>
#include <sstream>

namespace recongraph::crawler {

ReconCollector::ReconCollector(const ScanConfig& config) : config_(config) {}

void ReconCollector::cancel() {
    cancelled_.store(true);
}

bool ReconCollector::is_in_scope(const std::string& url) const {
    auto norm = core::UrlNormalizer::normalize(url);
    if (!norm) return false;

    if (config_.allowed_domains.empty()) {
        auto target = core::UrlNormalizer::normalize(config_.target_url);
        if (!target) return false;
        return norm->host == target->host;
    }

    for (const auto& domain : config_.allowed_domains) {
        if (norm->host == domain || 
            (norm->host.size() > domain.size() && 
             norm->host.compare(norm->host.size() - domain.size(), domain.size(), domain) == 0 &&
             norm->host[norm->host.size() - domain.size() - 1] == '.')) {
            return true;
        }
    }
    return false;
}

models::NodeType ReconCollector::classify_url(const std::string& path) const {
    std::string lower = path;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find("/admin") != std::string::npos ||
        lower.find("/dashboard") != std::string::npos ||
        lower.find("/manage") != std::string::npos) {
        return models::NodeType::AdminRoute;
    }
    if (lower.find("/login") != std::string::npos ||
        lower.find("/auth") != std::string::npos ||
        lower.find("/signin") != std::string::npos ||
        lower.find("/oauth") != std::string::npos) {
        return models::NodeType::AuthPoint;
    }
    if (lower.find("/api/") != std::string::npos ||
        lower.find("/v1/") != std::string::npos ||
        lower.find("/v2/") != std::string::npos ||
        lower.find(".json") != std::string::npos) {
        return models::NodeType::ApiEndpoint;
    }
    if (lower.find(".css") != std::string::npos ||
        lower.find(".js") != std::string::npos ||
        lower.find(".png") != std::string::npos ||
        lower.find(".jpg") != std::string::npos ||
        lower.find(".svg") != std::string::npos ||
        lower.find(".woff") != std::string::npos) {
        return models::NodeType::StaticResource;
    }
    return models::NodeType::Page;
}

std::vector<std::string> ReconCollector::extract_links(const std::string& html, const std::string& base_url) {
    std::vector<std::string> links;

    // Simple regex for href attributes (Phase 2 - good enough for controlled lab targets)
    std::regex href_regex(R"((?:href|src)\s*=\s*["']([^"']+)["'])", std::regex::icase);
    auto begin = std::sregex_iterator(html.begin(), html.end(), href_regex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::string raw = (*it)[1].str();
        if (raw.empty() || raw[0] == '#' || raw.find("javascript:") == 0 || raw.find("mailto:") == 0) {
            continue;
        }

        auto resolved = core::UrlNormalizer::resolve(base_url, raw);
        if (resolved) {
            links.push_back(*resolved);
        }
    }
    return links;
}

std::vector<std::pair<std::string, std::vector<std::string>>> ReconCollector::extract_forms(const std::string& html) {
    std::vector<std::pair<std::string, std::vector<std::string>>> forms;

    // Very lightweight form extraction for Phase 2
    std::regex form_regex(R"(<form[^>]*>([\s\S]*?)</form>)", std::regex::icase);
    std::regex action_regex(R"(action\s*=\s*["']([^"']*)["'])", std::regex::icase);
    std::regex input_regex(R"(<input[^>]*name\s*=\s*["']([^"']+)["'][^>]*>)", std::regex::icase);

    auto form_begin = std::sregex_iterator(html.begin(), html.end(), form_regex);
    auto form_end = std::sregex_iterator();

    for (auto it = form_begin; it != form_end; ++it) {
        std::string form_html = (*it)[0].str();
        std::string action;

        std::smatch action_match;
        if (std::regex_search(form_html, action_match, action_regex)) {
            action = action_match[1].str();
        }

        std::vector<std::string> fields;
        auto input_begin = std::sregex_iterator(form_html.begin(), form_html.end(), input_regex);
        auto input_end = std::sregex_iterator();
        for (auto iit = input_begin; iit != input_end; ++iit) {
            fields.push_back((*iit)[1].str());
        }

        forms.emplace_back(action, fields);
    }
    return forms;
}

graph::AttackSurfaceGraph ReconCollector::run(ProgressCallback on_progress) {
    graph::AttackSurfaceGraph graph;
    stats_ = ScanStats{};
    cancelled_.store(false);

    auto target_norm = core::UrlNormalizer::normalize(config_.target_url);
    if (!target_norm) {
        if (on_progress) on_progress("Invalid target URL", 0);
        return graph;
    }

    // Seed the graph with the root node
    models::GraphNode root;
    root.url = target_norm->canonical;
    root.type = models::NodeType::Root;
    root.method = "GET";
    root.discovery_source = "seed";
    root.depth = 0;
    std::uint64_t root_id = graph.add_node(root);

    // BFS queue: (url, depth, parent_id)
    struct QueueItem {
        std::string url;
        int depth;
        std::uint64_t parent_id;
    };

    dsa::Queue<QueueItem> frontier;
    frontier.enqueue({target_norm->canonical, 0, root_id});

    httplib::Client cli(target_norm->scheme + "://" + target_norm->host +
                        (target_norm->port > 0 ? ":" + std::to_string(target_norm->port) : ""));
    cli.set_connection_timeout(0, config_.request_timeout_ms * 1000);
    cli.set_read_timeout(0, config_.request_timeout_ms * 1000);

    int processed = 0;

    while (!frontier.empty() && !cancelled_.load() && processed < config_.max_requests) {
        QueueItem item = frontier.front();
        frontier.dequeue();

        if (item.depth > config_.max_depth) continue;

        auto norm = core::UrlNormalizer::normalize(item.url);
        if (!norm) continue;

        if (!is_in_scope(item.url)) {
            // Still record as external if desired
            if (config_.follow_external == false) {
                models::GraphNode ext;
                ext.url = norm->canonical;
                ext.type = models::NodeType::ExternalLink;
                ext.discovery_source = "link";
                ext.depth = item.depth;
                auto id = graph.add_node(ext);
                graph.add_edge(item.parent_id, id, "links_to");
            }
            continue;
        }

        // Skip if already visited (deduplication via graph hash table)
        if (graph.has_url(norm->canonical) && item.depth > 0) {
            ++stats_.duplicates_skipped;
            continue;
        }

        if (on_progress) {
            int pct = static_cast<int>((processed * 100.0) / config_.max_requests);
            on_progress("Fetching: " + norm->canonical, pct);
        }

        // Perform request (path + query only for httplib)
        std::string path = norm->path;
        if (!norm->query.empty()) path += "?" + norm->query;

        auto res = cli.Get(path.c_str());
        ++processed;
        ++stats_.pages_visited;

        if (!res || res->status >= 400) {
            ++stats_.errors;
            continue;
        }

        // Create / update node
        models::GraphNode node;
        node.url = norm->canonical;
        node.type = (item.depth == 0) ? models::NodeType::Root : classify_url(norm->path);
        node.method = "GET";
        node.discovery_source = (item.depth == 0) ? "seed" : "crawl";
        node.depth = item.depth;
        node.status_code = res->status;

        // Extract query parameter names
        auto param_names = core::UrlNormalizer::extract_param_names(norm->query);
        node.parameters = param_names;
        stats_.parameters_found += static_cast<int>(param_names.size());

        std::uint64_t node_id;
        if (item.depth == 0) {
            node_id = root_id;
            // Update root with status
            auto* existing = graph.get_node(root_id);
            if (existing) {
                existing->status_code = res->status;
                existing->parameters = param_names;
            }
        } else {
            node_id = graph.add_node(node);
            graph.add_edge(item.parent_id, node_id, "navigates_to");
        }

        // Extract links and forms from HTML
        if (res->body.find("<html") != std::string::npos ||
            res->body.find("<HTML") != std::string::npos ||
            res->body.find("<!DOCTYPE") != std::string::npos) {

            auto links = extract_links(res->body, norm->canonical);
            stats_.links_discovered += static_cast<int>(links.size());

            for (const auto& link : links) {
                if (cancelled_.load()) break;
                if (!graph.has_url(link)) {
                    frontier.enqueue({link, item.depth + 1, node_id});
                } else {
                    // Still record the relationship
                    auto* existing = graph.find_by_url(link);
                    if (existing) {
                        graph.add_edge(node_id, existing->id, "navigates_to");
                    }
                }
            }

            auto forms = extract_forms(res->body);
            stats_.forms_found += static_cast<int>(forms.size());

            for (const auto& [action, fields] : forms) {
                auto form_url = core::UrlNormalizer::resolve(norm->canonical, action.empty() ? norm->path : action);
                if (!form_url) continue;

                models::GraphNode form_node;
                form_node.url = *form_url + "#form";
                form_node.type = models::NodeType::Form;
                form_node.method = "POST"; // assumption for Phase 2
                form_node.discovery_source = "form";
                form_node.depth = item.depth;
                form_node.parameters = fields;

                auto form_id = graph.add_node(form_node);
                graph.add_edge(node_id, form_id, "contains_form");

                stats_.parameters_found += static_cast<int>(fields.size());
            }
        }
    }

    if (on_progress) {
        on_progress(cancelled_.load() ? "Scan cancelled" : "Scan complete", 100);
    }

    return graph;
}

} // namespace recongraph::crawler
