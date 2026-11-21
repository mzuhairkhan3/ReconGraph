# ReconGraph – Demo & Walkthrough

This guide walks you through a complete demonstration of ReconGraph suitable for portfolio reviews, demos, or lab exercises.

## Prerequisites

- Node.js 18+
- CMake 3.16+
- C++17 compiler (g++, clang++, or MSVC)
- A local web target (any simple HTTP server is fine)

## 1. Start the Backend

```bash
cd backend
mkdir -p build && cd build
cmake ..
cmake --build .
./recongraph_server
```

You should see:

```
========================================
  ReconGraph Backend v0.2.0
  Attack-Surface Intelligence Engine
  Phase 2 - Core Engine
========================================
[ReconGraph] Starting server on http://localhost:8080
```

Health check:

```bash
curl http://localhost:8080/api/health
```

## 2. Start the Frontend

```bash
cd frontend
npm install
npm run dev
```

Open **http://localhost:5173**

## 3. Prepare a Lab Target

Any local site works. Examples:

```bash
# Simple Python server
python3 -m http.server 3000

# or npx serve
npx serve -l 3000
```

## 4. Run a Scan from the UI

1. Go to **New Reconnaissance**
2. Target: `http://localhost:3000`
3. Max Depth: `2`
4. Request Limit: `20–30`
5. Click **Start Reconnaissance**

Watch the backend console for progress messages.

## 5. Explore Results

| Page | What to show |
|------|--------------|
| **Dashboard** | Live stats (pages, nodes, forms, links) |
| **Attack Surface Map** | 3D graph – drag to rotate, scroll to zoom, click nodes |
| **Endpoints** | Table of discovered routes |
| **Analysis & Prioritization** | Ranked testing opportunities with scores |
| **Reports** | Download JSON report |

## 6. API-only Demo (optional)

```bash
# Normalize a URL
curl -X POST http://localhost:8080/api/normalize \
  -H "Content-Type: application/json" \
  -d '{"url":"http://Example.com:80/path/"}'

# Run scan
curl -X POST http://localhost:8080/api/scan \
  -H "Content-Type: application/json" \
  -d '{"url":"http://localhost:3000","max_depth":2,"max_requests":15}'

# Inspect graph
curl http://localhost:8080/api/graph | jq

# Stats
curl http://localhost:8080/api/stats
```

## Talking Points for Interviews / Reviews

1. **Custom DSA** – HashTable for deduplication, Queue for BFS crawl, DynamicArray for adjacency lists, Stack for DFS.
2. **Responsible design** – depth limits, request limits, no brute-force, no credential attacks, explainable scores that never claim “vulnerable”.
3. **Graph model** – nodes typed (root, page, API, form, auth, admin…), edges express relationships.
4. **3D visualization** – React Three Fiber, interactive selection, color-coded node types.
5. **C++ + React architecture** – clear separation, REST boundary, portfolio-ready structure.

## Known Limitations (be transparent)

- HTML parsing is lightweight (regex-based) – good for demos, not a full browser engine.
- 3D layout is radial/spiral (force-directed planned as future work).
- Persistence is currently file-backed; SQLite schema is ready for full integration.
- Only authorized / lab targets should be scanned.
