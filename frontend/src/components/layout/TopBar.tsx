import { Bell, Circle } from 'lucide-react'

export function TopBar() {
  return (
    <header className="h-14 flex-shrink-0 border-b border-border bg-surface-50 flex items-center justify-between px-6">
      <div className="flex items-center gap-4">
        <div className="flex items-center gap-2 text-sm">
          <Circle className="w-2.5 h-2.5 fill-accent-green text-accent-green" />
          <span className="text-text-secondary">Backend</span>
          <span className="text-text-muted">·</span>
          <span className="text-text-secondary">Idle</span>
        </div>
      </div>

      <div className="flex items-center gap-4">
        <div className="text-sm text-text-muted">
          No active target
        </div>
        <button className="p-2 rounded-md text-text-secondary hover:bg-surface-100 hover:text-text-primary transition-colors">
          <Bell className="w-4 h-4" />
        </button>
        <div className="w-8 h-8 rounded-full bg-surface-200 border border-border flex items-center justify-center text-xs text-text-secondary">
          U
        </div>
      </div>
    </header>
  )
}
