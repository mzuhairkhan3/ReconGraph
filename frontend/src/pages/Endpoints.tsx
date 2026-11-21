import { useEffect, useState } from 'react'
import { api, GraphSummary } from '../services/api'

export function Endpoints() {
  const [graph, setGraph] = useState<GraphSummary | null>(null)

  useEffect(() => {
    api.graph().then(setGraph).catch(() => setGraph(null))
  }, [])

  const endpoints = graph?.nodes.filter(n =>
    ['page', 'api_endpoint', 'admin_route', 'auth_point', 'root'].includes(n.type)
  ) || []

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">Endpoints</h1>
        <p className="mt-1 text-text-secondary text-sm">Discovered routes and API endpoints</p>
      </div>

      <div className="card overflow-hidden">
        {endpoints.length === 0 ? (
          <p className="text-text-muted text-sm p-4">No endpoints discovered yet. Run a reconnaissance scan.</p>
        ) : (
          <table className="w-full text-sm">
            <thead>
              <tr className="border-b border-border text-left text-text-muted">
                <th className="px-4 py-2.5 font-medium">Method</th>
                <th className="px-4 py-2.5 font-medium">URL</th>
                <th className="px-4 py-2.5 font-medium">Type</th>
              </tr>
            </thead>
            <tbody>
              {endpoints.map((n) => (
                <tr key={n.id} className="border-b border-border/50 hover:bg-surface-100">
                  <td className="px-4 py-2 font-mono text-xs text-accent-cyan">{n.method}</td>
                  <td className="px-4 py-2 font-mono text-xs text-text-primary truncate max-w-md">{n.url}</td>
                  <td className="px-4 py-2 text-text-secondary">{n.type}</td>
                </tr>
              ))}
            </tbody>
          </table>
        )}
      </div>
    </div>
  )
}
