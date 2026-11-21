# ReconGraph

**Attack-Surface Intelligence & Reconnaissance Engine**

A professional-grade offensive-security reconnaissance platform designed for authorized penetration testing and red-team assessments. ReconGraph discovers, organizes, visualizes, and prioritizes the attack surface of web applications by transforming raw reconnaissance data into an interactive 3D attack-surface graph.

> **Status:** Phases 1–6 complete  
> Portfolio-ready foundation with custom DSA, controlled recon, graph engine, prioritization, and 3D visualization.

---

## Features

- **Controlled reconnaissance collector** – depth-limited, request-limited, scope-aware crawling
- **Custom DSA** – DynamicArray, HashTable, Queue, Stack, LinkedList used in real code paths
- **Attack-surface graph engine** – adjacency-list graph with BFS/DFS
- **URL normalization & deduplication**
- **Explainable prioritization** – scores testing interest without claiming vulnerabilities
- **3D visualization** – React Three Fiber interactive graph
- **Dark cybersecurity UI** with Martel typography
- **JSON report export**
- **REST API** (C++ / cpp-httplib)

---

## Technology Stack

| Layer | Tech |
|-------|------|
| Backend | C++17, CMake, cpp-httplib |
| Frontend | React 18, TypeScript, Vite, Tailwind, React Three Fiber |
| Typography | Martel |
| Persistence | File-backed session store + SQLite schema (ready for full amalgamation) |
| DSA | Custom implementations under `dsa/` |

---

## Quick Start

### Backend
```bash
cd backend
mkdir -p build && cd build
cmake ..
cmake --build .
./recongraph_server          # listens on :8080
```

### Frontend
```bash
cd frontend
npm install
npm run dev                  # http://localhost:5173
```

### Helper script
```bash
./scripts/dev.sh backend     # build + run C++ server
./scripts/dev.sh frontend    # start Vite
```

### Example scan
```bash
curl -X POST http://localhost:8080/api/scan \
  -H "Content-Type: application/json" \
  -d '{"url":"http://localhost:3000","max_depth":2,"max_requests":20}'
```

Full walkthrough: **[docs/user_guide/DEMO.md](docs/user_guide/DEMO.md)**

---

## Project Structure

```
ReconGraph/
├── frontend/          # React + TypeScript + R3F
├── backend/           # C++ engine + REST API
├── dsa/               # Custom data structures & algorithms
├── database/          # Schema reference
├── docs/              # Architecture, demo guide, algorithms
├── tests/             # Integration checklist + benchmark notes
└── scripts/           # dev helpers
```

---

## Documentation

| Doc | Description |
|-----|-------------|
| [Demo Guide](docs/user_guide/DEMO.md) | Step-by-step demo for portfolio / interviews |
| [Architecture](docs/architecture/OVERVIEW.md) | System design and data flow |
| [DSA Overview](docs/algorithms/dsa_overview.md) | Custom structures and complexity |
| [Integration Checklist](tests/integration/README.md) | Manual verification steps |

---

## Security & Responsible Use

ReconGraph is intended **only for authorized testing**.  
It enforces request/depth limits and is designed for lab and explicitly scoped targets.

**Do not point this tool at systems you do not own or have written permission to test.**

---

## License

MIT – see [LICENSE](LICENSE)

---

Built by [mzuhairkhan3](https://github.com/mzuhairkhan3)
