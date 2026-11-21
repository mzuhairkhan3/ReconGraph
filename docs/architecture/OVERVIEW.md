# ReconGraph Architecture Overview

## High-Level Design

```
┌─────────────────────────────────────────────────────────────┐
│                        Frontend (React)                     │
│  Dashboard · New Recon · 3D Map · Endpoints · Analysis      │
│              Tailwind + Martel + React Three Fiber          │
└────────────────────────────┬────────────────────────────────┘
                             │ REST (JSON)
                             ▼
┌─────────────────────────────────────────────────────────────┐
│                     Backend (C++17)                         │
│  cpp-httplib server                                         │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────────┐   │
│  │  Collector  │→ │ Graph Engine │→ │  Prioritizer     │   │
│  │  (BFS)      │  │ (Adj. List)  │  │  + Reporter      │   │
│  └─────────────┘  └──────────────┘  └──────────────────┘   │
│         │                │                                  │
│         ▼                ▼                                  │
│  Custom DSA (Queue, HashTable, DynamicArray, Stack…)        │
│  URL Normalizer · Scope checks · File-backed sessions       │
└─────────────────────────────────────────────────────────────┘
```

## Backend Modules

| Module | Responsibility |
|--------|----------------|
| `core/url_normalizer` | Canonicalization, relative resolution, param extraction |
| `crawler/recon_collector` | Bounded BFS crawl, link/form extraction, scope enforcement |
| `graph/attack_surface_graph` | Node/edge storage, BFS/DFS, JSON export |
| `analyzer/prioritizer` | Explainable scoring of testing interest |
| `analyzer/report_generator` | JSON + HTML reports |
| `database` | Session + graph persistence (file-backed + SQLite schema) |
| `api/server` | REST endpoints, CORS, request handling |

## Data Flow (Scan)

1. Client `POST /api/scan` with target + limits
2. Collector seeds graph with root node
3. BFS frontier (custom Queue) processes pages
4. URL normalizer + HashTable prevent duplicates
5. Links/forms become new nodes + edges
6. Graph returned; prioritizer can rank nodes
7. Frontend fetches `/api/graph` and `/api/stats`

## Frontend Structure

- `services/api.ts` – typed client for all backend endpoints
- `components/graph/AttackSurfaceCanvas.tsx` – R3F 3D scene
- Pages are thin; data comes from the API (no hardcoded demo values on Dashboard after a scan)

## Security Posture

- Request and depth ceilings
- Scope checking (same-host by default)
- No authentication bypass, no credential stuffing, no destructive payloads
- Scores describe *testing priority*, never vulnerability certainty
