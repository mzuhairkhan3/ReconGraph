#pragma once

#include "models/node.hpp"
#include "dsa/data_structures/dynamic_array/dynamic_array.hpp"
#include "dsa/data_structures/hash_table/hash_table.hpp"
#include "dsa/data_structures/queue/queue.hpp"
#include "dsa/data_structures/stack/stack.hpp"

#include <string>
#include <vector>
#include <optional>
#include <functional>

namespace recongraph::graph {

/**
 * Core Attack-Surface Graph Engine.
 *
 * Representation: Adjacency list using custom DynamicArray.
 * Nodes and edges are stored and traversed with custom DSA.
 */
class AttackSurfaceGraph {
public:
    AttackSurfaceGraph() = default;

    // Node operations
    std::uint64_t add_node(const models::GraphNode& node);
    bool remove_node(std::uint64_t id);
    models::GraphNode* get_node(std::uint64_t id);
    const models::GraphNode* get_node(std::uint64_t id) const;
    models::GraphNode* find_by_url(const std::string& canonical_url);
    bool has_url(const std::string& canonical_url) const;

    // Edge operations
    bool add_edge(std::uint64_t from, std::uint64_t to, const std::string& relation);
    std::vector<std::uint64_t> get_neighbors(std::uint64_t id) const;
    std::vector<models::GraphEdge> get_edges_from(std::uint64_t id) const;

    // Graph properties
    std::size_t node_count() const { return nodes_.size(); }
    std::size_t edge_count() const { return edge_count_; }
    bool empty() const { return nodes_.empty(); }

    // Traversal (using custom Queue / Stack)
    void bfs(std::uint64_t start_id, const std::function<void(const models::GraphNode&)>& visitor) const;
    void dfs(std::uint64_t start_id, const std::function<void(const models::GraphNode&)>& visitor) const;

    // Filtering helpers
    std::vector<const models::GraphNode*> nodes_of_type(models::NodeType type) const;
    std::vector<const models::GraphNode*> all_nodes() const;

    // Export
    std::string to_json_summary() const;

    void clear();

private:
    // Node storage (index == id for simplicity in Phase 2)
    dsa::DynamicArray<models::GraphNode> nodes_;

    // Adjacency list: node_id -> list of (neighbor_id, relation)
    dsa::DynamicArray<dsa::DynamicArray<std::pair<std::uint64_t, std::string>>> adj_;

    // Fast URL -> node_id lookup (custom hash table)
    dsa::HashTable<std::string, std::uint64_t> url_index_;

    std::size_t edge_count_ = 0;
    std::uint64_t next_id_ = 0;
};

} // namespace recongraph::graph
