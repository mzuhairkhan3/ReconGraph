export function Dashboard() {
  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight text-text-primary">
          Dashboard
        </h1>
        <p className="mt-1 text-text-secondary text-sm">
          Attack-surface overview and recent activity
        </p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        {['Pages', 'Endpoints', 'Parameters', 'Forms'].map((label) => (
          <div key={label} className="card">
            <div className="text-text-muted text-xs uppercase tracking-wider">{label}</div>
            <div className="mt-2 text-2xl font-semibold text-text-primary">—</div>
            <div className="mt-1 text-xs text-text-muted">Awaiting first scan</div>
          </div>
        ))}
      </div>

      <div className="card">
        <h2 className="text-sm font-medium text-text-secondary mb-3">Status</h2>
        <p className="text-text-muted text-sm">
          Phase 1 foundation is live. Backend health endpoint and frontend shell are ready.
          Core reconnaissance, graph engine, and 3D visualization will be implemented in subsequent phases.
        </p>
      </div>
    </div>
  )
}
