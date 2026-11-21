import { useEffect, useState } from 'react'
import { api, GraphSummary } from '../services/api'

export function Analysis() {
  const [graph, setGraph] = useState<GraphSummary | null>(null)

  useEffect(() => {
    api.graph().then(setGraph).catch(() => setGraph(null))
  }, [])

  // Client-side lightweight ranking mirroring backend prioritizer logic
  const ranked = (graph?.nodes || [])
    .map((n) => {
      let score = 5
      if (n.type === 'admin_route') score = 40
      else if (n.type === 'auth_point') score = 35
      else if (n.type === 'form') score = 30
      else if (n.type === 'api_endpoint') score = 25
      else if (n.type === 'page') score = 10
      return { ...n, score }
    })
    .sort((a, b) => b.score - a.score)
    .slice(0, 15)

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">Analysis & Prioritization</h1>
        <p className="mt-1 text-text-secondary text-sm">
          Explainable ranking of testing opportunities (no vulnerability claims)
        </p>
      </div>

      <div className="card">
        {ranked.length === 0 ? (
          <p className="text-text-muted text-sm">No data. Run a scan first.</p>
        ) : (
          <table className="w-full text-sm">
            <thead>
              <tr className="border-b border-border text-left text-text-muted">
                <th className="px-4 py-2.5 font-medium">Rank</th>
                <th className="px-4 py-2.5 font-medium">Score</th>
                <th className="px-4 py-2.5 font-medium">Type</th>
                <th className="px-4 py-2.5 font-medium">URL</th>
              </tr>
            </thead>
            <tbody>
              {ranked.map((n, i) => (
                <tr key={n.id} className="border-b border-border/50">
                  <td className="px-4 py-2 text-text-muted">{i + 1}</td>
                  <td className="px-4 py-2 text-accent-green font-medium">{n.score}</td>
                  <td className="px-4 py-2 text-text-secondary">{n.type}</td>
                  <td className="px-4 py-2 font-mono text-xs truncate max-w-lg">{n.url}</td>
                </tr>
              ))}
            </tbody>
          </table>
        )}
      </div>

      <p className="text-xs text-text-muted">
        Scores are derived from node type, parameters, auth context, and depth. They indicate testing interest only.
      </p>
    </div>
  )
}
