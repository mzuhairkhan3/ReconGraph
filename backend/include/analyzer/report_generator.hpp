#pragma once

#include "graph/attack_surface_graph.hpp"
#include "crawler/recon_collector.hpp"
#include "analyzer/prioritizer.hpp"

#include <string>

namespace recongraph::analyzer {

class ReportGenerator {
public:
    ReportGenerator(const graph::AttackSurfaceGraph& graph,
                    const crawler::ScanStats& stats,
                    const std::string& target_url);

    std::string to_json() const;
    std::string to_html() const;

private:
    const graph::AttackSurfaceGraph& graph_;
    const crawler::ScanStats& stats_;
    std::string target_url_;
};

} // namespace recongraph::analyzer
