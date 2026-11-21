#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace recongraph::models {

enum class NodeType {
    Root,
    Page,
    ApiEndpoint,
    Form,
    Parameter,
    AuthPoint,
    AdminRoute,
    ExternalLink,
    StaticResource,
    Unknown
};

inline std::string node_type_to_string(NodeType t) {
    switch (t) {
        case NodeType::Root:           return "root";
        case NodeType::Page:           return "page";
        case NodeType::ApiEndpoint:    return "api_endpoint";
        case NodeType::Form:           return "form";
        case NodeType::Parameter:      return "parameter";
        case NodeType::AuthPoint:      return "auth_point";
        case NodeType::AdminRoute:     return "admin_route";
        case NodeType::ExternalLink:   return "external_link";
        case NodeType::StaticResource: return "static_resource";
        default:                       return "unknown";
    }
}

struct GraphNode {
    std::uint64_t id = 0;
    std::string url;                 // Canonical URL or identifier
    NodeType type = NodeType::Unknown;
    std::string method = "GET";      // HTTP method if applicable
    std::string discovery_source;    // How this node was found
    int depth = 0;
    bool requires_auth = false;
    double priority_score = 0.0;
    std::string priority_reason;

    // Metadata
    std::vector<std::string> parameters;
    std::string content_type;
    int status_code = 0;
};

struct GraphEdge {
    std::uint64_t from_id = 0;
    std::uint64_t to_id = 0;
    std::string relation;            // "navigates_to", "submits_to", "includes", "param_of", etc.
};

} // namespace recongraph::models
