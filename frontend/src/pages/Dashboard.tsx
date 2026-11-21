import { useEffect, useState } from 'react'
import { api, GraphSummary, Stats } from '../services/api'
import { Activity, Network, Link2, FormInput, AlertCircle } from 'lucide-react'

export function Dashboard() {
  const [stats, setStats] = useState<Stats | null>(null)
  const [graph, setGraph] = useState<GraphSummary | null>(null)
  const [health, setHealth] = useState<string>('checking…')
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    let cancelled = false

    async function load() {
      try {
        const h = await api.health()
        if (!cancelled) setHealth(`${h.status} · ${h.phase}`)

        const [s, g] = await Promise.all([api.stats(), api.graph()])
        if (!cancelled) {
          setStats(s)
          setGraph(g)
        }
      } catch (e: any) {
        if (!cancelled) {
          setHealth('unreachable')
          setError('Backend not reachable. Start the C++ server on port 8080.')
        }
      }
    }

    load()
    const id = setInterval(load, 8000)
    return () => {
      cancelled = true
      clearInterval(id)
    }
  }, [])

  const cards = [
    { label: 'Pages Visited', value: stats?.pages_visited ?? '—', icon: Activity },
    { label: 'Graph Nodes', value: graph?.node_count ?? '—', icon: Network },
    { label: 'Links Found', value: stats?.links_discovered ?? '—', icon: Link2 },
    { label: 'Forms', value: stats?.forms_found ?? '—', icon: FormInput },
  ]

  return (
    <div className="space-y-6">
      <div className="flex items-start justify-between">
        <div>
          <h1 className="text-2xl font-semibold tracking-tight text-text-primary">Dashboard</h1>
          <p className="mt-1 text-text-secondary text-sm">Attack-surface overview and recent activity</p>
        </div>
        <div className="text-xs text-text-muted border border-border rounded px-2.5 py-1">
          Backend: {health}
        </div>
      </div>

      {error && (
        <div className="flex items-center gap-2 text-sm text-accent-red bg-accent-red/5 border border-accent-red/20 rounded-md px-3 py-2">
          <AlertCircle className="w-4 h-4 flex-shrink-0" />
          {error}
        </div>
      )}

      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        {cards.map(({ label, value, icon: Icon }) => (
          <div key={label} className="card">
            <div className="flex items-center justify-between">
              <div className="text-text-muted text-xs uppercase tracking-wider">{label}</div>
              <Icon className="w-4 h-4 text-text-muted" />
            </div>
            <div className="mt-2 text-2xl font-semibold text-text-primary">{value}</div>
          </div>
        ))}
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
        <div className="card">
          <h2 className="text-sm font-medium text-text-secondary mb-3">Latest Graph Snapshot</h2>
          {graph && graph.nodes.length > 0 ? (
            <ul className="space-y-1.5 max-h-64 overflow-y-auto text-sm">
              {graph.nodes.slice(0, 12).map((n) => (
                <li key={n.id} className="flex items-center justify-between gap-2">
                  <span className="truncate text-text-primary font-mono text-xs">{n.url}</span>
                  <span className="text-text-muted text-xs flex-shrink-0">{n.type}</span>
                </li>
              ))}
            </ul>
          ) : (
            <p className="text-text-muted text-sm">No scan data yet. Run a reconnaissance from the New Recon page.</p>
          )}
        </div>

        <div className="card">
          <h2 className="text-sm font-medium text-text-secondary mb-3">Scan Statistics</h2>
          {stats ? (
            <dl className="grid grid-cols-2 gap-x-4 gap-y-2 text-sm">
              <dt className="text-text-muted">Parameters</dt>
              <dd className="text-text-primary">{stats.parameters_found}</dd>
              <dt className="text-text-muted">Duplicates skipped</dt>
              <dd className="text-text-primary">{stats.duplicates_skipped}</dd>
              <dt className="text-text-muted">Errors</dt>
              <dd className="text-text-primary">{stats.errors}</dd>
            </dl>
          ) : (
            <p className="text-text-muted text-sm">Awaiting first scan.</p>
          )}
        </div>
      </div>
    </div>
  )
}
