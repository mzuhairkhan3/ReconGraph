#pragma once

#include <string>
#include <vector>
#include <optional>

namespace recongraph::core {

struct NormalizedUrl {
    std::string scheme;      // http / https
    std::string host;
    int port = -1;           // -1 means default
    std::string path;        // always starts with /
    std::string query;       // without leading ?
    std::string fragment;    // without leading #

    // Canonical string used for deduplication
    std::string canonical;
};

/**
 * URL Normalization utilities.
 *
 * Responsibilities:
 * - Normalize trailing slashes
 * - Lowercase scheme and host
 * - Remove default ports
 * - Consistent path representation
 * - Separate query parameter names from values where useful
 * - Produce a stable canonical form for hash-table deduplication
 */
class UrlNormalizer {
public:
    /**
     * Parse and normalize a raw URL string.
     * Returns nullopt if the URL is invalid or unsupported.
     */
    static std::optional<NormalizedUrl> normalize(const std::string& raw_url);

    /**
     * Extract only the path + query portion (useful for relative links).
     */
    static std::string normalize_path(const std::string& path);

    /**
     * Resolve a relative URL against a base URL.
     */
    static std::optional<std::string> resolve(const std::string& base, const std::string& relative);

    /**
     * Check whether two URLs are considered equivalent after normalization.
     */
    static bool are_equivalent(const std::string& a, const std::string& b);

    /**
     * Extract parameter names from a query string (values discarded for grouping).
     */
    static std::vector<std::string> extract_param_names(const std::string& query);

private:
    static std::string to_lower(const std::string& s);
    static std::string strip_default_port(const std::string& scheme, const std::string& host_port);
    static std::string normalize_path_internal(const std::string& path);
};

} // namespace recongraph::core
