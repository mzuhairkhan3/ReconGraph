#include "analyzer/prioritizer.hpp"

#include <algorithm>
#include <sstream>

namespace recongraph::analyzer {

Prioritizer::Prioritizer(const graph::AttackSurfaceGraph& graph) : graph_(graph) {}

double Prioritizer::base_score_for_type(models::NodeType t) const {
    switch (t) {
        case models::NodeType::AdminRoute:     return 40.0;
        case models::NodeType::AuthPoint:      return 35.0;
        case models::NodeType::Form:           return 30.0;
        case models::NodeType::ApiEndpoint:    return 25.0;
        case models::NodeType::Parameter:      return 15.0;
        case models::NodeType::Page:           return 10.0;
        case models::NodeType::Root:           return 5.0;
        case models::NodeType::ExternalLink:   return 2.0;
        case models::NodeType::StaticResource: return 1.0;
        default:                               return 5.0;
    }
}

PriorityResult Prioritizer::score_node(const models::GraphNode& node) const {
    PriorityResult r;
    r.node_id = node.id;
    r.url = node.url;
    r.type = node.type;
    r.score = base_score_for_type(node.type);

    std::ostringstream reason;
    reason << "Base score for type '" << models::node_type_to_string(node.type) << "' = "
           << base_score_for_type(node.type) << ". ";

    // Parameter presence
    if (!node.parameters.empty()) {
        double bonus = std::min(20.0, node.parameters.size() * 4.0);
        r.score += bonus;
        r.factors.push_back("parameters");
        reason << "Contains " << node.parameters.size()
               << " parameter(s) (+" << bonus << "). ";
    }

    // Authentication context
    if (node.requires_auth) {
        r.score += 15.0;
        r.factors.push_back("requires_auth");
        reason << "Marked as requiring authentication (+15). ";
    }

    // Admin / auth indicators already reflected in type, but reinforce
    if (node.type == models::NodeType::AdminRoute) {
        r.factors.push_back("admin_indicator");
        reason << "Administrative functionality indicator. ";
    }
    if (node.type == models::NodeType::AuthPoint) {
        r.factors.push_back("auth_endpoint");
        reason << "Authentication-related endpoint. ";
    }

    // Forms are high-value for input testing
    if (node.type == models::NodeType::Form) {
        r.factors.push_back("form_submission");
        reason << "Form submission point – prioritise input validation testing. ";
    }

    // Shallower nodes are often more critical entry points
    if (node.depth <= 1) {
        r.score += 5.0;
        r.factors.push_back("shallow_depth");
        reason << "Shallow depth (entry-point proximity) (+5). ";
    }

    // Discovery source
    if (node.discovery_source == "form") {
        r.score += 5.0;
        r.factors.push_back("discovered_via_form");
    }

    r.reason = reason.str();
    return r;
}

std::vector<PriorityResult> Prioritizer::rank(int top_n) const {
    std::vector<PriorityResult> results;

    for (const auto* node : graph_.all_nodes()) {
        if (!node) continue;
        results.push_back(score_node(*node));
    }

    std::sort(results.begin(), results.end(),
              [](const PriorityResult& a, const PriorityResult& b) {
                  return a.score > b.score;
              });

    if (static_cast<int>(results.size()) > top_n) {
        results.resize(top_n);
    }
    return results;
}

} // namespace recongraph::analyzer
