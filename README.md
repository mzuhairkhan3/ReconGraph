# ReconGraph

**Attack-Surface Intelligence & Reconnaissance Engine**

A professional-grade offensive-security reconnaissance platform designed for authorized penetration testing and red-team assessments. ReconGraph discovers, organizes, visualizes, and prioritizes the attack surface of web applications by transforming raw reconnaissance data into an interactive 3D attack-surface graph.

> **Status:** Phase 1 – Foundation & Project Structure  
> This repository is under active development following a structured, incremental approach.

---

## Overview

ReconGraph is **not** a basic website crawler. It is an intelligent reconnaissance platform that uses custom Data Structures and Algorithms to model discovered web application components as a rich, interactive graph.

**Key Capabilities (Planned / In Progress):**
- Controlled, scope-aware reconnaissance collector
- Custom graph engine representing pages, endpoints, forms, parameters, and relationships
- Interactive 3D attack-surface visualization (React Three Fiber)
- Explainable prioritization of testing opportunities
- SQLite persistence with full scan history
- Professional dark cybersecurity UI with Martel typography
- Meaningful custom DSA implementations used in production code paths

---

## Technology Stack

### Backend
- **Language:** C++17 / C++20
- **Build:** CMake
- **HTTP Server:** [cpp-httplib](https://github.com/yhirose/cpp-httplib) (header-only)
- **Database:** SQLite3
- **Architecture:** Modular REST API + core engine (crawler, parser, graph, algorithms, analyzer)

### Frontend
- **Framework:** React 18 + TypeScript
- **Build Tool:** Vite
- **Styling:** Tailwind CSS
- **3D Visualization:** React Three Fiber + Three.js
- **Icons:** Lucide React
- **Typography:** Martel (primary font across the entire application)

### Data & Storage
- SQLite for scan sessions, endpoints, parameters, graph nodes/edges, and history
- JSON import/export support

---

## Project Structure

```
ReconGraph/
├── frontend/                 # React + TypeScript + Vite application
├── backend/                  # C++ core engine + REST API (CMake)
├── dsa/                      # Custom Data Structures & Algorithms implementations
├── database/                 # SQLite schema and migrations
├── docs/                     # Architecture, API, algorithms, user guide
├── tests/                    # Integration tests, benchmarks, test data
├── scripts/                  # Helper scripts
├── .gitignore
├── README.md
├── LICENSE
└── CONTRIBUTING.md
```

Detailed module breakdown lives inside each directory.

---

## Getting Started (Phase 1)

### Prerequisites
- Node.js 18+
- CMake 3.16+
- C++17-compatible compiler (GCC, Clang, or MSVC)
- Git

### Frontend

```bash
cd frontend
npm install
npm run dev
```

Frontend will be available at `http://localhost:5173`

### Backend

```bash
cd backend
mkdir build && cd build
cmake ..
cmake --build .
./recongraph_server
```

Backend API will listen on `http://localhost:8080` by default.

---

## Development Phases

| Phase | Focus |
|-------|-------|
| **1** | Project structure, CMake + Vite foundation, basic server & frontend shell |
| **2** | Core C++ DSA, URL normalization, reconnaissance collector, graph engine |
| **3** | REST API, SQLite integration, frontend ↔ backend connection |
| **4** | Dashboard, 3D attack-surface visualization, endpoint/parameter views |
| **5** | Prioritization engine, reporting, scan history |
| **6** | Testing, performance, documentation, polish |

---

## Security & Responsible Use

ReconGraph is designed **exclusively for authorized testing**.  
It includes:
- Explicit target allowlisting
- Scope validation
- Rate limiting & depth limits
- Localhost / private network restrictions by default

**Do not use this tool against systems you do not own or have explicit permission to test.**

---

## License

MIT License – see [LICENSE](LICENSE)

---

## Author

Built by [mzuhairkhan3](https://github.com/mzuhairkhan3) as a professional cybersecurity portfolio project.
