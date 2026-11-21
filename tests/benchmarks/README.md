# Benchmarks

This directory is reserved for performance comparisons between custom DSA implementations and standard-library equivalents.

## Planned / Recommended Benchmarks

1. **HashTable vs `std::unordered_map`**
   - Insert + lookup of 10k–100k normalized URLs
2. **DynamicArray vs `std::vector`**
   - Push-back growth behaviour and random access
3. **BFS crawl cost**
   - Time to traverse synthetic graphs of increasing size
4. **Prioritizer ranking**
   - Sort 1k–10k scored nodes

## How to add a benchmark

1. Create a small C++ executable under `tests/benchmarks/`
2. Link against the same headers used by the backend
3. Use `std::chrono` for timing
4. Record results in a markdown table in this file

Example skeleton:

```cpp
#include <chrono>
#include <iostream>
#include "dsa/data_structures/hash_table/hash_table.hpp"

int main() {
    recongraph::dsa::HashTable<std::string, int> ht;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000; ++i) {
        ht.insert("url-" + std::to_string(i), i);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
}
```

Results can be pasted below as the suite grows.
