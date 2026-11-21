const API_BASE = '/api';

export interface HealthResponse {
  status: string;
  service: string;
  version: string;
  phase: string;
  message: string;
}

export interface ScanRequest {
  url: string;
  max_depth?: number;
  max_requests?: number;
}

export interface ScanResult {
  status: string;
  target: string;
  nodes: number;
  edges: number;
  pages_visited: number;
  links_discovered: number;
  forms_found: number;
  parameters_found: number;
  duplicates_skipped: number;
  errors: number;
}

export interface GraphNode {
  id: number;
  url: string;
  type: string;
  method: string;
}

export interface GraphSummary {
  node_count: number;
  edge_count: number;
  nodes: GraphNode[];
}

export interface Stats {
  pages_visited: number;
  links_discovered: number;
  forms_found: number;
  parameters_found: number;
  duplicates_skipped: number;
  errors: number;
}

async function request<T>(path: string, options?: RequestInit): Promise<T> {
  const res = await fetch(`${API_BASE}${path}`, {
    headers: { 'Content-Type': 'application/json', ...(options?.headers || {}) },
    ...options,
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(text || `HTTP ${res.status}`);
  }
  return res.json();
}

export const api = {
  health: () => request<HealthResponse>('/health'),

  normalize: (url: string) =>
    request<any>('/normalize', { method: 'POST', body: JSON.stringify({ url }) }),

  scan: (payload: ScanRequest) =>
    request<ScanResult>('/scan', {
      method: 'POST',
      body: JSON.stringify(payload),
    }),

  graph: () => request<GraphSummary>('/graph'),

  stats: () => request<Stats>('/stats'),
};
