import { useEffect, useState } from 'react'
import { api, GraphSummary } from '../services/api'
import { AttackSurfaceCanvas } from '../components/graph/AttackSurfaceCanvas'
import { NODE_COLORS } from '../types'

export function AttackSurfaceMap() {
  const [graph, setGraph] = useState<GraphSummary | null>(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    api.graph()
      .then(setGraph)
      .catch(() => setGraph(null))
      .finally(() => setLoading(false))
  }, [])

  return (
    <div className="h-[calc(100vh-8rem)] flex flex-col space-y-4">
      <div className="flex items-start justify-between">
        <div>
          <h1 className="text-2xl font-semibold tracking-tight">Attack Surface Map</h1>
          <p className="mt-1 text-text-secondary text-sm">
            Interactive 3D visualization of discovered resources
          </p>
        </div>
        <div className="flex flex-wrap gap-2 text-[10px]">
          {Object.entries(NODE_COLORS).slice(0, 6).map(([type, color]) => (
            <div key={type} className="flex items-center gap-1.5">
              <span className="w-2.5 h-2.5 rounded-full" style={{ background: color }} />
              <span className="text-text-muted">{type}</span>
            </div>
          ))}
        </div>
      </div>

      <div className="flex-1 min-h-0 card p-0 overflow-hidden relative">
        {loading ? (
          <div className="h-full flex items-center justify-center text-text-muted text-sm">Loading graph…</div>
        ) : (
          <AttackSurfaceCanvas nodes={graph?.nodes || []} />
        )}
      </div>

      <p className="text-xs text-text-muted">
        Drag to rotate · Scroll to zoom · Click a node to inspect. Layout will become force-directed in a later iteration.
      </p>
    </div>
  )
}
