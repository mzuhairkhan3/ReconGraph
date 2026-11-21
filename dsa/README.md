# Data Structures & Algorithms

Custom implementations used by the ReconGraph backend. These are intentional educational + production-path structures, not thin wrappers around the STL.

## Implemented Structures

| Structure | Path | Used For |
|-----------|------|----------|
| **DynamicArray** | `data_structures/dynamic_array/` | Graph node storage, adjacency lists |
| **LinkedList** | `data_structures/linked_list/` | Chronological scan events (ready) |
| **HashTable** | `data_structures/hash_table/` | URL / endpoint deduplication (chaining + rehash) |
| **Queue** | `data_structures/queue/` | BFS crawl frontier |
| **Stack** | `data_structures/stack/` | DFS graph traversal |

## Graph Algorithms

| Algorithm | Where | Purpose |
|-----------|-------|---------|
| BFS | `AttackSurfaceGraph::bfs` | Crawl order & reachability |
| DFS | `AttackSurfaceGraph::dfs` | Alternative traversal |

## Complexity (typical)

- DynamicArray append: amortised **O(1)** (doubling strategy)
- HashTable insert/lookup: average **O(1)**, worst **O(n)** (chaining)
- Queue / Stack push & pop: **O(1)**
- BFS / DFS over graph: **O(V + E)**

## Why custom?

1. Demonstrates real understanding of memory layout, growth strategies, and collision handling.
2. Structures are wired into the actual crawler and graph engine, not only mentioned in docs.
3. Makes performance trade-offs visible for coursework and portfolio discussion.

See also: `docs/algorithms/dsa_overview.md` and `tests/benchmarks/`.
