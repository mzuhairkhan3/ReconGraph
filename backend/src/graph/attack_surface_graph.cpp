#include "graph/attack_surface_graph.hpp"

#include <sstream>
#include <unordered_set>

namespace recongraph::graph {

std::uint64_t AttackSurfaceGraph::add_node(const models::GraphNode& node) {
    models::GraphNode n = node;
    n.id = next_id_++;

    // Ensure storage capacity
    while (nodes_.size() <= n.id) {
        nodes_.push_back(models::GraphNode{});
        adj_.push_back(dsa::DynamicArray<std::pair<std::uint64_t, std::string>>{});
    }

    nodes_[n.id] = n;
    url_index_.insert(n.url, n.id);

    return n.id;
}

bool AttackSurfaceGraph::remove_node(std::uint64_t id) {
    if (id >= nodes_.size()) return false;

    url_index_.erase(nodes_[id].url);
    // Simple soft-delete for Phase 2 (keep indices stable)
    nodes_[id] = models::GraphNode{};
    adj_[id] = dsa::DynamicArray<std::pair<std::uint64_t, std::string>>{};
    return true;
}

models::GraphNode* AttackSurfaceGraph::get_node(std::uint64_t id) {
    if (id >= nodes_.size()) return nullptr;
    if (nodes_[id].url.empty()) return nullptr; // soft-deleted
    return &nodes_[id];
}

const models::GraphNode* AttackSurfaceGraph::get_node(std::uint64_t id) const {
    if (id >= nodes_.size()) return nullptr;
    if (nodes_[id].url.empty()) return nullptr;
    return &nodes_[id];
}

models::GraphNode* AttackSurfaceGraph::find_by_url(const std::string& canonical_url) {
    auto* id_ptr = url_index_.find(canonical_url);
    if (!id_ptr) return nullptr;
    return get_node(*id_ptr);
}

bool AttackSurfaceGraph::has_url(const std::string& canonical_url) const {
    return url_index_.contains(canonical_url);
}

bool AttackSurfaceGraph::add_edge(std::uint64_t from, std::uint64_t to, const std::string& relation) {
    if (from >= adj_.size() || to >= nodes_.size()) return false;
    if (nodes_[from].url.empty() || nodes_[to].url.empty()) return false;

    adj_[from].push_back({to, relation});
    ++edge_count_;
    return true;
}

std::vector<std::uint64_t> AttackSurfaceGraph::get_neighbors(std::uint64_t id) const {
    std::vector<std::uint64_t> result;
    if (id >= adj_.size()) return result;
    for (std::size_t i = 0; i < adj_[id].size(); ++i) {
        result.push_back(adj_[id][i].first);
    }
    return result;
}

std::vector<models::GraphEdge> AttackSurfaceGraph::get_edges_from(std::uint64_t id) const {
    std::vector<models::GraphEdge> result;
    if (id >= adj_.size()) return result;
    for (std::size_t i = 0; i < adj_[id].size(); ++i) {
        models::GraphEdge e;
        e.from_id = id;
        e.to_id = adj_[id][i].first;
        e.relation = adj_[id][i].second;
        result.push_back(e);
    }
    return result;
}

void AttackSurfaceGraph::bfs(std::uint64_t start_id,
                             const std::function<void(const models::GraphNode&)>& visitor) const {
    if (!get_node(start_id)) return;

    dsa::Queue<std::uint64_t> q;
    std::unordered_set<std::uint64_t> visited;

    q.enqueue(start_id);
    visited.insert(start_id);

    while (!q.empty()) {
        std::uint64_t current = q.front();
        q.dequeue();

        const auto* node = get_node(current);
        if (node) visitor(*node);

        for (std::uint64_t neighbor : get_neighbors(current)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.enqueue(neighbor);
            }
        }
    }
}

void AttackSurfaceGraph::dfs(std::uint64_t start_id,
                             const std::function<void(const models::GraphNode&)>& visitor) const {
    if (!get_node(start_id)) return;

    dsa::Stack<std::uint64_t> st;
    std::unordered_set<std::uint64_t> visited;

    st.push(start_id);

    while (!st.empty()) {
        std::uint64_t current = st.top();
        st.pop();

        if (visited.count(current)) continue;
        visited.insert(current);

        const auto* node = get_node(current);
        if (node) visitor(*node);

        // Push neighbors in reverse so order feels more natural
        auto neighbors = get_neighbors(current);
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            if (!visited.count(*it)) {
                st.push(*it);
            }
        }
    }
}

std::vector<const models::GraphNode*> AttackSurfaceGraph::nodes_of_type(models::NodeType type) const {
    std::vector<const models::GraphNode*> result;
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        if (!nodes_[i].url.empty() && nodes_[i].type == type) {
            result.push_back(&nodes_[i]);
        }
    }
    return result;
}

std::vector<const models::GraphNode*> AttackSurfaceGraph::all_nodes() const {
    std::vector<const models::GraphNode*> result;
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        if (!nodes_[i].url.empty()) {
            result.push_back(&nodes_[i]);
        }
    }
    return result;
}

std::string AttackSurfaceGraph::to_json_summary() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"node_count\": " << node_count() << ",\n";
    oss << "  \"edge_count\": " << edge_count() << ",\n";
    oss << "  \"nodes\": [\n";

    bool first = true;
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i].url.empty()) continue;
        if (!first) oss << ",\n";
        first = false;
        oss << "    {\"id\": " << nodes_[i].id
            << ", \"url\": \"" << nodes_[i].url
            << "\", \"type\": \"" << models::node_type_to_string(nodes_[i].type)
            << "\", \"method\": \"" << nodes_[i].method << "\"}";
    }
    oss << "\n  ]\n}";
    return oss.str();
}

void AttackSurfaceGraph::clear() {
    nodes_ = dsa::DynamicArray<models::GraphNode>{};
    adj_ = dsa::DynamicArray<dsa::DynamicArray<std::pair<std::uint64_t, std::string>>>{};
    url_index_.clear();
    edge_count_ = 0;
    next_id_ = 0;
}

} // namespace recongraph::graph
