import { useState } from 'react'
import { api, ScanResult } from '../services/api'
import { Radar, Loader2 } from 'lucide-react'

export function NewRecon() {
  const [url, setUrl] = useState('http://localhost:3000')
  const [maxDepth, setMaxDepth] = useState(2)
  const [maxRequests, setMaxRequests] = useState(25)
  const [loading, setLoading] = useState(false)
  const [result, setResult] = useState<ScanResult | null>(null)
  const [error, setError] = useState<string | null>(null)

  async function handleScan(e: React.FormEvent) {
    e.preventDefault()
    setLoading(true)
    setError(null)
    setResult(null)

    try {
      const res = await api.scan({
        url,
        max_depth: maxDepth,
        max_requests: maxRequests,
      })
      setResult(res)
    } catch (err: any) {
      setError(err.message || 'Scan failed. Is the backend running?')
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="space-y-6 max-w-2xl">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">New Reconnaissance</h1>
        <p className="mt-1 text-text-secondary text-sm">
          Configure and launch an authorized, scope-limited scan
        </p>
      </div>

      <form onSubmit={handleScan} className="card space-y-4">
        <div>
          <label className="block text-sm text-text-secondary mb-1.5">Target URL</label>
          <input
            type="url"
            required
            value={url}
            onChange={(e) => setUrl(e.target.value)}
            placeholder="http://localhost:3000"
            className="w-full bg-surface border border-border rounded-md px-3 py-2 text-sm text-text-primary placeholder:text-text-muted focus:outline-none focus:border-accent-cyan/50"
          />
          <p className="mt-1.5 text-xs text-text-muted">
            Prefer localhost / private lab targets. External targets should only be used with explicit authorization.
          </p>
        </div>

        <div className="grid grid-cols-2 gap-4">
          <div>
            <label className="block text-sm text-text-secondary mb-1.5">Max Depth</label>
            <input
              type="number"
              min={0}
              max={5}
              value={maxDepth}
              onChange={(e) => setMaxDepth(Number(e.target.value))}
              className="w-full bg-surface border border-border rounded-md px-3 py-2 text-sm"
            />
          </div>
          <div>
            <label className="block text-sm text-text-secondary mb-1.5">Request Limit</label>
            <input
              type="number"
              min={1}
              max={100}
              value={maxRequests}
              onChange={(e) => setMaxRequests(Number(e.target.value))}
              className="w-full bg-surface border border-border rounded-md px-3 py-2 text-sm"
            />
          </div>
        </div>

        <button
          type="submit"
          disabled={loading}
          className="btn-primary flex items-center gap-2 disabled:opacity-50"
        >
          {loading ? (
            <>
              <Loader2 className="w-4 h-4 animate-spin" />
              Scanning…
            </>
          ) : (
            <>
              <Radar className="w-4 h-4" />
              Start Reconnaissance
            </>
          )}
        </button>
      </form>

      {error && (
        <div className="text-sm text-accent-red bg-accent-red/5 border border-accent-red/20 rounded-md px-3 py-2">
          {error}
        </div>
      )}

      {result && (
        <div className="card space-y-3">
          <h2 className="text-sm font-medium text-accent-green">Scan completed</h2>
          <dl className="grid grid-cols-2 gap-x-4 gap-y-1.5 text-sm">
            <dt className="text-text-muted">Target</dt>
            <dd className="text-text-primary font-mono text-xs truncate">{result.target}</dd>
            <dt className="text-text-muted">Nodes</dt>
            <dd className="text-text-primary">{result.nodes}</dd>
            <dt className="text-text-muted">Edges</dt>
            <dd className="text-text-primary">{result.edges}</dd>
            <dt className="text-text-muted">Pages visited</dt>
            <dd className="text-text-primary">{result.pages_visited}</dd>
            <dt className="text-text-muted">Forms</dt>
            <dd className="text-text-primary">{result.forms_found}</dd>
            <dt className="text-text-muted">Parameters</dt>
            <dd className="text-text-primary">{result.parameters_found}</dd>
            <dt className="text-text-muted">Duplicates skipped</dt>
            <dd className="text-text-primary">{result.duplicates_skipped}</dd>
          </dl>
          <p className="text-xs text-text-muted pt-2">
            View the full graph on the Attack Surface Map page.
          </p>
        </div>
      )}
    </div>
  )
}
