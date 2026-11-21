# Data Structures & Algorithms in ReconGraph

This document summarises the custom DSA implementations and where they are used.

## Data Structures

| Structure | Location | Primary Use | Time Complexity (typical) |
|-----------|----------|-------------|---------------------------|
| DynamicArray | `dsa/data_structures/dynamic_array/` | Node storage, adjacency lists | Amortised O(1) append, O(1) index |
| LinkedList | `dsa/data_structures/linked_list/` | Scan event history (ready) | O(1) push/pop front/back |
| HashTable | `dsa/data_structures/hash_table/` | URL / endpoint deduplication | Average O(1) lookup/insert |
| Queue | `dsa/data_structures/queue/` | BFS crawl frontier | O(1) enqueue/dequeue |
| Stack | `dsa/data_structures/stack/` | DFS graph traversal | O(1) push/pop |

## Graph Algorithms

| Algorithm | Implementation | Use |
|-----------|----------------|-----|
| BFS | `AttackSurfaceGraph::bfs` + custom Queue | Crawling order, reachability |
| DFS | `AttackSurfaceGraph::dfs` + custom Stack | Alternative traversal / analysis |

## Design Notes

- Custom structures are used in the hot paths of the crawler and graph engine, not merely declared.
- HashTable uses separate chaining and automatic rehashing at load factor 0.75.
- DynamicArray doubles capacity on growth (classic amortised analysis).
- Prioritization uses a transparent scoring model; scores are explainable and never claim vulnerabilities.

Further benchmarks comparing custom vs `std::` containers can be added under `tests/benchmarks/`.
