export function Settings() {
  return (
    <div className="space-y-6 max-w-xl">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">Settings</h1>
        <p className="mt-1 text-text-secondary text-sm">Application and scan defaults</p>
      </div>
      <div className="card space-y-3">
        <div className="text-sm text-text-secondary">Backend URL</div>
        <div className="text-sm font-mono text-text-muted">http://localhost:8080</div>
        <div className="pt-2 text-xs text-text-muted">
          Scope enforcement, rate limits, and lab configuration will be exposed here in later phases.
        </div>
      </div>
    </div>
  )
}
