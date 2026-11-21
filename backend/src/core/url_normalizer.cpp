#include "core/url_normalizer.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace recongraph::core {

std::string UrlNormalizer::to_lower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

std::string UrlNormalizer::normalize_path_internal(const std::string& path) {
    if (path.empty()) return "/";

    std::string result;
    result.reserve(path.size());

    // Simple path normalization: collapse multiple slashes, ensure leading /
    bool last_was_slash = false;
    for (char c : path) {
        if (c == '/') {
            if (!last_was_slash) {
                result.push_back('/');
                last_was_slash = true;
            }
        } else {
            result.push_back(c);
            last_was_slash = false;
        }
    }

    if (result.empty()) result = "/";

    // Remove trailing slash except for root
    if (result.size() > 1 && result.back() == '/') {
        result.pop_back();
    }

    return result;
}

std::optional<NormalizedUrl> UrlNormalizer::normalize(const std::string& raw_url) {
    if (raw_url.empty()) return std::nullopt;

    NormalizedUrl out;
    std::string url = raw_url;

    // Find scheme
    auto scheme_end = url.find("://");
    if (scheme_end == std::string::npos) {
        // Assume relative or add default scheme later
        return std::nullopt;
    }

    out.scheme = to_lower(url.substr(0, scheme_end));
    if (out.scheme != "http" && out.scheme != "https") {
        return std::nullopt;
    }

    std::size_t pos = scheme_end + 3;

    // Find host (and optional port)
    std::size_t path_start = url.find('/', pos);
    std::size_t query_start = url.find('?', pos);
    std::size_t frag_start = url.find('#', pos);

    std::size_t host_end = url.size();
    if (path_start != std::string::npos) host_end = std::min(host_end, path_start);
    if (query_start != std::string::npos) host_end = std::min(host_end, query_start);
    if (frag_start != std::string::npos) host_end = std::min(host_end, frag_start);

    std::string host_port = url.substr(pos, host_end - pos);
    if (host_port.empty()) return std::nullopt;

    // Split host and port
    auto colon = host_port.rfind(':');
    if (colon != std::string::npos && host_port.find(']') == std::string::npos) {
        // Simple IPv4 / hostname:port
        out.host = to_lower(host_port.substr(0, colon));
        try {
            out.port = std::stoi(host_port.substr(colon + 1));
        } catch (...) {
            return std::nullopt;
        }
    } else {
        out.host = to_lower(host_port);
        out.port = -1;
    }

    // Default ports
    if ((out.scheme == "http" && out.port == 80) ||
        (out.scheme == "https" && out.port == 443)) {
        out.port = -1;
    }

    // Path
    if (path_start != std::string::npos) {
        std::size_t path_end = url.size();
        if (query_start != std::string::npos) path_end = std::min(path_end, query_start);
        if (frag_start != std::string::npos) path_end = std::min(path_end, frag_start);
        out.path = normalize_path_internal(url.substr(path_start, path_end - path_start));
    } else {
        out.path = "/";
    }

    // Query
    if (query_start != std::string::npos) {
        std::size_t query_end = (frag_start != std::string::npos) ? frag_start : url.size();
        out.query = url.substr(query_start + 1, query_end - query_start - 1);
    }

    // Fragment (usually ignored for attack-surface identity)
    if (frag_start != std::string::npos) {
        out.fragment = url.substr(frag_start + 1);
    }

    // Build canonical form (used for deduplication)
    std::ostringstream canon;
    canon << out.scheme << "://" << out.host;
    if (out.port > 0) canon << ":" << out.port;
    canon << out.path;
    if (!out.query.empty()) canon << "?" << out.query;

    out.canonical = canon.str();
    return out;
}

std::string UrlNormalizer::normalize_path(const std::string& path) {
    return normalize_path_internal(path);
}

std::optional<std::string> UrlNormalizer::resolve(const std::string& base, const std::string& relative) {
    auto base_norm = normalize(base);
    if (!base_norm) return std::nullopt;

    if (relative.empty()) return base_norm->canonical;

    // Absolute URL
    if (relative.find("://") != std::string::npos) {
        auto rel_norm = normalize(relative);
        return rel_norm ? std::optional<std::string>(rel_norm->canonical) : std::nullopt;
    }

    // Protocol-relative
    if (relative.size() >= 2 && relative[0] == '/' && relative[1] == '/') {
        return normalize(base_norm->scheme + ":" + relative)->canonical;
    }

    std::string new_path;
    if (!relative.empty() && relative[0] == '/') {
        // Absolute path on same host
        new_path = normalize_path_internal(relative);
    } else {
        // Relative path
        std::string base_path = base_norm->path;
        if (base_path.empty() || base_path.back() != '/') {
            // Remove last segment
            auto last_slash = base_path.rfind('/');
            if (last_slash != std::string::npos) {
                base_path = base_path.substr(0, last_slash + 1);
            } else {
                base_path = "/";
            }
        }
        new_path = normalize_path_internal(base_path + relative);
    }

    std::ostringstream resolved;
    resolved << base_norm->scheme << "://" << base_norm->host;
    if (base_norm->port > 0) resolved << ":" << base_norm->port;
    resolved << new_path;

    return resolved.str();
}

bool UrlNormalizer::are_equivalent(const std::string& a, const std::string& b) {
    auto na = normalize(a);
    auto nb = normalize(b);
    if (!na || !nb) return false;
    return na->canonical == nb->canonical;
}

std::vector<std::string> UrlNormalizer::extract_param_names(const std::string& query) {
    std::vector<std::string> names;
    if (query.empty()) return names;

    std::size_t start = 0;
    while (start < query.size()) {
        std::size_t amp = query.find('&', start);
        std::string pair = query.substr(start, amp == std::string::npos ? std::string::npos : amp - start);

        auto eq = pair.find('=');
        std::string name = (eq == std::string::npos) ? pair : pair.substr(0, eq);
        if (!name.empty()) {
            names.push_back(name);
        }

        if (amp == std::string::npos) break;
        start = amp + 1;
    }
    return names;
}

} // namespace recongraph::core
