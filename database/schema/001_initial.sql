-- ReconGraph SQLite Schema (Phase 3)
-- Attack-Surface Intelligence Engine

PRAGMA foreign_keys = ON;

-- Projects (optional multi-project support)
CREATE TABLE IF NOT EXISTS projects (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT NOT NULL,
    description     TEXT,
    created_at      TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at      TEXT NOT NULL DEFAULT (datetime('now'))
);

-- Scan sessions
CREATE TABLE IF NOT EXISTS scan_sessions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id      INTEGER REFERENCES projects(id) ON DELETE SET NULL,
    target_url      TEXT NOT NULL,
    max_depth       INTEGER NOT NULL DEFAULT 3,
    max_requests    INTEGER NOT NULL DEFAULT 100,
    status          TEXT NOT NULL DEFAULT 'pending', -- pending, running, completed, cancelled, failed
    started_at      TEXT,
    finished_at     TEXT,
    pages_visited   INTEGER DEFAULT 0,
    links_discovered INTEGER DEFAULT 0,
    forms_found     INTEGER DEFAULT 0,
    parameters_found INTEGER DEFAULT 0,
    duplicates_skipped INTEGER DEFAULT 0,
    errors          INTEGER DEFAULT 0,
    notes           TEXT,
    created_at      TEXT NOT NULL DEFAULT (datetime('now'))
);

-- Graph nodes
CREATE TABLE IF NOT EXISTS graph_nodes (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id      INTEGER NOT NULL REFERENCES scan_sessions(id) ON DELETE CASCADE,
    node_key        TEXT NOT NULL,          -- canonical URL or identifier
    node_type       TEXT NOT NULL,          -- root, page, api_endpoint, form, etc.
    method          TEXT DEFAULT 'GET',
    discovery_source TEXT,
    depth           INTEGER DEFAULT 0,
    requires_auth   INTEGER DEFAULT 0,
    priority_score  REAL DEFAULT 0.0,
    priority_reason TEXT,
    status_code     INTEGER,
    content_type    TEXT,
    parameters_json TEXT,                   -- JSON array of parameter names
    created_at      TEXT NOT NULL DEFAULT (datetime('now')),
    UNIQUE(session_id, node_key)
);

-- Graph edges
CREATE TABLE IF NOT EXISTS graph_edges (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id      INTEGER NOT NULL REFERENCES scan_sessions(id) ON DELETE CASCADE,
    from_node_id    INTEGER NOT NULL REFERENCES graph_nodes(id) ON DELETE CASCADE,
    to_node_id      INTEGER NOT NULL REFERENCES graph_nodes(id) ON DELETE CASCADE,
    relation        TEXT NOT NULL,          -- navigates_to, contains_form, links_to, etc.
    created_at      TEXT NOT NULL DEFAULT (datetime('now'))
);

-- Scan events (chronological log)
CREATE TABLE IF NOT EXISTS scan_events (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id      INTEGER NOT NULL REFERENCES scan_sessions(id) ON DELETE CASCADE,
    event_type      TEXT NOT NULL,          -- info, warning, error, discovery
    message         TEXT NOT NULL,
    created_at      TEXT NOT NULL DEFAULT (datetime('now'))
);

-- Indexes for common queries
CREATE INDEX IF NOT EXISTS idx_nodes_session ON graph_nodes(session_id);
CREATE INDEX IF NOT EXISTS idx_nodes_type ON graph_nodes(node_type);
CREATE INDEX IF NOT EXISTS idx_edges_session ON graph_edges(session_id);
CREATE INDEX IF NOT EXISTS idx_events_session ON scan_events(session_id);
CREATE INDEX IF NOT EXISTS idx_sessions_status ON scan_sessions(status);
