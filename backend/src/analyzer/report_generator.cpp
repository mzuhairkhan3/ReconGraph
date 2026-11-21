#include "analyzer/report_generator.hpp"

#include <sstream>
#include <iomanip>

namespace recongraph::analyzer {

ReportGenerator::ReportGenerator(const graph::AttackSurfaceGraph& graph,
                                 const crawler::ScanStats& stats,
                                 const std::string& target_url)
    : graph_(graph), stats_(stats), target_url_(target_url) {}

std::string ReportGenerator::to_json() const {
    Prioritizer prioritizer(graph_);
    auto ranked = prioritizer.rank(15);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    oss << "{\n"
        << "  \"target\": \"" << target_url_ << "\",\n"
        << "  \"summary\": {\n"
        << "    \"nodes\": " << graph_.node_count() << ",\n"
        << "    \"edges\": " << graph_.edge_count() << ",\n"
        << "    \"pages_visited\": " << stats_.pages_visited << ",\n"
        << "    \"forms_found\": " << stats_.forms_found << ",\n"
        << "    \"parameters_found\": " << stats_.parameters_found << "\n"
        << "  },\n"
        << "  \"prioritized_targets\": [\n";

    for (std::size_t i = 0; i < ranked.size(); ++i) {
        const auto& r = ranked[i];
        oss << "    {\n"
            << "      \"rank\": " << (i + 1) << ",\n"
            << "      \"url\": \"" << r.url << "\",\n"
            << "      \"type\": \"" << models::node_type_to_string(r.type) << "\",\n"
            << "      \"score\": " << r.score << ",\n"
            << "      \"reason\": \"" << r.reason << "\"\n"
            << "    }";
        if (i + 1 < ranked.size()) oss << ",";
        oss << "\n";
    }

    oss << "  ],\n"
        << "  \"limitations\": [\n"
        << "    \"This is a reconnaissance report only. No vulnerability claims are made.\",\n"
        << "    \"Manual verification is required for all prioritized targets.\",\n"
        << "    \"Scan was bounded by depth and request limits.\"\n"
        << "  ]\n"
        << "}\n";

    return oss.str();
}

std::string ReportGenerator::to_html() const {
    Prioritizer prioritizer(graph_);
    auto ranked = prioritizer.rank(10);

    std::ostringstream oss;
    oss << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>ReconGraph Report</title>"
        << "<style>"
        << "body{font-family:Martel,Georgia,serif;background:#0a0e17;color:#e2e8f0;padding:2rem;}"
        << "h1,h2{color:#22d3ee;} table{border-collapse:collapse;width:100%;}"
        << "th,td{border:1px solid #1e293b;padding:0.5rem 0.75rem;text-align:left;}"
        << "th{background:#111827;}"
        << ".score{color:#34d399;font-weight:600;}"
        << "</style></head><body>"
        << "<h1>ReconGraph Attack-Surface Report</h1>"
        << "<p><strong>Target:</strong> " << target_url_ << "</p>"
        << "<h2>Summary</h2><ul>"
        << "<li>Nodes: " << graph_.node_count() << "</li>"
        << "<li>Edges: " << graph_.edge_count() << "</li>"
        << "<li>Pages visited: " << stats_.pages_visited << "</li>"
        << "<li>Forms found: " << stats_.forms_found << "</li>"
        << "</ul>"
        << "<h2>Prioritized Testing Opportunities</h2>"
        << "<table><thead><tr><th>Rank</th><th>URL</th><th>Type</th><th>Score</th><th>Reason</th></tr></thead><tbody>";

    for (std::size_t i = 0; i < ranked.size(); ++i) {
        const auto& r = ranked[i];
        oss << "<tr><td>" << (i + 1) << "</td><td>" << r.url
            << "</td><td>" << models::node_type_to_string(r.type)
            << "</td><td class=\"score\">" << r.score
            << "</td><td>" << r.reason << "</td></tr>";
    }

    oss << "</tbody></table>"
        << "<h2>Limitations</h2><ul>"
        << "<li>Reconnaissance only – no vulnerability claims.</li>"
        << "<li>Manual verification required.</li>"
        << "<li>Bounded by configured depth and request limits.</li>"
        << "</ul></body></html>";

    return oss.str();
}

} // namespace recongraph::analyzer
