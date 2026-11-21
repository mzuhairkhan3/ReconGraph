export function NewRecon() {
  return (
    <div className="space-y-6 max-w-2xl">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">New Reconnaissance</h1>
        <p className="mt-1 text-text-secondary text-sm">
          Configure and launch an authorized scan
        </p>
      </div>

      <div className="card space-y-4">
        <div>
          <label className="block text-sm text-text-secondary mb-1.5">Target URL</label>
          <input
            type="url"
            placeholder="https://example.com or http://localhost:3000"
            className="w-full bg-surface border border-border rounded-md px-3 py-2 text-sm text-text-primary placeholder:text-text-muted focus:outline-none focus:border-accent-cyan/50"
            disabled
          />
        </div>
        <div className="grid grid-cols-2 gap-4">
          <div>
            <label className="block text-sm text-text-secondary mb-1.5">Max Depth</label>
            <input type="number" defaultValue={3} disabled className="w-full bg-surface border border-border rounded-md px-3 py-2 text-sm" />
          </div>
          <div>
            <label className="block text-sm text-text-secondary mb-1.5">Request Limit</label>
            <input type="number" defaultValue={100} disabled className="w-full bg-surface border border-border rounded-md px-3 py-2 text-sm" />
          </div>
        </div>
        <button className="btn-primary opacity-60 cursor-not-allowed" disabled>
          Start Reconnaissance (Phase 2+)
        </button>
      </div>
    </div>
  )
}
