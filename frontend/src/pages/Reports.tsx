import { useState } from 'react'
import { api } from '../services/api'
import { FileText, Download } from 'lucide-react'

export function Reports() {
  const [busy, setBusy] = useState(false)
  const [message, setMessage] = useState<string | null>(null)

  async function downloadJson() {
    setBusy(true)
    setMessage(null)
    try {
      const [graph, stats] = await Promise.all([api.graph(), api.stats()])
      const report = {
        generated_at: new Date().toISOString(),
        summary: stats,
        graph,
        note: 'Reconnaissance report only. No vulnerability claims.',
      }
      const blob = new Blob([JSON.stringify(report, null, 2)], { type: 'application/json' })
      const url = URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = `recongraph-report-${Date.now()}.json`
      a.click()
      URL.revokeObjectURL(url)
      setMessage('JSON report downloaded.')
    } catch {
      setMessage('Failed to generate report. Is the backend running and has a scan completed?')
    } finally {
      setBusy(false)
    }
  }

  return (
    <div className="space-y-6 max-w-xl">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">Reports</h1>
        <p className="mt-1 text-text-secondary text-sm">Export scan results for documentation</p>
      </div>

      <div className="card space-y-4">
        <div className="flex items-start gap-3">
          <FileText className="w-5 h-5 text-accent-cyan mt-0.5" />
          <div>
            <div className="text-sm font-medium text-text-primary">JSON Report</div>
            <p className="text-xs text-text-muted mt-0.5">
              Includes graph summary, statistics, and metadata. Suitable for archival and further analysis.
            </p>
          </div>
        </div>

        <button
          onClick={downloadJson}
          disabled={busy}
          className="btn-primary flex items-center gap-2 disabled:opacity-50"
        >
          <Download className="w-4 h-4" />
          {busy ? 'Generating…' : 'Download JSON Report'}
        </button>

        {message && <p className="text-xs text-text-secondary">{message}</p>}
      </div>
    </div>
  )
}
