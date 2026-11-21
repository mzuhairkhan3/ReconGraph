import { useEffect, useState } from 'react'
import { api, GraphSummary } from '../services/api'

export function Parameters() {
  const [graph, setGraph] = useState<GraphSummary | null>(null)

  useEffect(() => {
    api.graph().then(setGraph).catch(() => setGraph(null))
  }, [])

  // Nodes that typically carry parameters (forms + anything with query potential)
  const withParams = (graph?.nodes || []).filter(
    (n) => n.type === 'form' || n.type === 'api_endpoint' || n.url.includes('?')
  )

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">Parameters</h1>
        <p className="mt-1 text-text-secondary text-sm">
          Query parameters and form fields discovered during reconnaissance
        </p>
      </div>

      <div className="card overflow-hidden">
        {withParams.length === 0 ? (
          <p className="text-text-muted text-sm p-4">
            No parameter-bearing endpoints discovered yet. Run a scan against a target that uses forms or query strings.
          </p>
        ) : (
          <table className="w-full text-sm">
            <thead>
              <tr className="border-b border-border text-left text-text-muted">
                <th className="px-4 py-2.5 font-medium">Type</th>
                <th className="px-4 py-2.5 font-medium">URL / Form</th>
                <th className="px-4 py-2.5 font-medium">Method</th>
              </tr>
            </thead>
            <tbody>
              {withParams.map((n) => (
                <tr key={n.id} className="border-b border-border/50 hover:bg-surface-100">
                  <td className="px-4 py-2 text-text-secondary">{n.type}</td>
                  <td className="px-4 py-2 font-mono text-xs text-text-primary truncate max-w-lg">{n.url}</td>
                  <td className="px-4 py-2 font-mono text-xs text-accent-cyan">{n.method}</td>
                </tr>
              ))}
            </tbody>
          </table>
        )}
      </div>
    </div>
  )
}
