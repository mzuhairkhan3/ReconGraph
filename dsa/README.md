# Data Structures & Algorithms

This directory contains **custom implementations** of core data structures and algorithms used by the ReconGraph backend.

These are not wrappers around the C++ Standard Library. They are intentional implementations that demonstrate understanding of time/space complexity and are integrated into real application paths (endpoint storage, URL deduplication, graph traversal, prioritization, etc.).

## Structure

```
dsa/
├── data_structures/
│   ├── dynamic_array/
│   ├── linked_list/
│   ├── hash_table/
│   ├── queue/
│   ├── stack/
│   ├── binary_search_tree/
│   └── priority_queue/
├── algorithms/
│   ├── bfs/
│   ├── dfs/
│   ├── sorting/
│   ├── string_matching/
│   └── graph_analysis/
└── README.md
```

## Usage in ReconGraph

| Structure / Algorithm | Primary Use |
|-----------------------|-------------|
| Dynamic Array | Endpoint lists, graph node storage |
| Linked List | Scan event history, chronological records |
| Hash Table | URL/endpoint deduplication, fast lookup |
| Queue | BFS crawling, pending page processing |
| Stack | DFS traversal, navigation history |
| BST / AVL | Ordered endpoint index |
| Priority Queue | Highest-priority testing opportunities |
| BFS / DFS | Graph traversal & reachability |
| Sorting | Priority and discovery-time ordering |
| String Matching | Route and parameter search |

Implementation and detailed complexity analysis will be added in Phase 2.
