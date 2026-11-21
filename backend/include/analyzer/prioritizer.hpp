#pragma once

#include "models/node.hpp"
#include "graph/attack_surface_graph.hpp"

#include <string>
#include <vector>

namespace recongraph::analyzer {

struct PriorityResult {
    std::uint64_t node_id;
    std::string url;
    models::NodeType type;
    double score;
    std::string reason;          // human-readable explanation
    std::vector<std::string> factors;
};

/**
 * Explainable prioritization engine.
 *
 * Scores are derived from observable facts only:
 * - presence of parameters
 * - authentication context
 * - administrative indicators
 * - form submission points
 * - depth / discovery source
 *
 * Never claims a node is "vulnerable". Only ranks testing interest.
 */
class Prioritizer {
public:
    explicit Prioritizer(const graph::AttackSurfaceGraph& graph);

    std::vector<PriorityResult> rank(int top_n = 20) const;

    // Score a single node and return explanation
    PriorityResult score_node(const models::GraphNode& node) const;

private:
    const graph::AttackSurfaceGraph& graph_;

    double base_score_for_type(models::NodeType t) const;
};

} // namespace recongraph::analyzer
