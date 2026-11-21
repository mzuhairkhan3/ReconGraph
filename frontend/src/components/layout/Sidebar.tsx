import { NavLink } from 'react-router-dom'
import {
  LayoutDashboard,
  Radar,
  Network,
  Link2,
  FormInput,
  BarChart3,
  History,
  FileText,
  Settings,
} from 'lucide-react'

const navItems = [
  { to: '/', label: 'Dashboard', icon: LayoutDashboard },
  { to: '/new-recon', label: 'New Reconnaissance', icon: Radar },
  { to: '/map', label: 'Attack Surface Map', icon: Network },
  { to: '/endpoints', label: 'Endpoints', icon: Link2 },
  { to: '/parameters', label: 'Parameters', icon: FormInput },
  { to: '/analysis', label: 'Analysis & Prioritization', icon: BarChart3 },
  { to: '/history', label: 'Scan History', icon: History },
  { to: '/reports', label: 'Reports', icon: FileText },
  { to: '/settings', label: 'Settings', icon: Settings },
]

export function Sidebar() {
  return (
    <aside className="w-64 flex-shrink-0 border-r border-border bg-surface-50 flex flex-col">
      <div className="h-14 flex items-center px-5 border-b border-border">
        <div className="flex items-center gap-2">
          <div className="w-8 h-8 rounded bg-accent-cyan/20 border border-accent-cyan/40 flex items-center justify-center">
            <Network className="w-4 h-4 text-accent-cyan" />
          </div>
          <span className="font-semibold text-lg tracking-tight text-text-primary">
            ReconGraph
          </span>
        </div>
      </div>

      <nav className="flex-1 py-4 px-3 space-y-0.5 overflow-y-auto">
        {navItems.map(({ to, label, icon: Icon }) => (
          <NavLink
            key={to}
            to={to}
            end={to === '/'}
            className={({ isActive }) =>
              `flex items-center gap-3 px-3 py-2.5 rounded-md text-sm transition-colors ${
                isActive
                  ? 'bg-accent-cyan/10 text-accent-cyan border border-accent-cyan/20'
                  : 'text-text-secondary hover:bg-surface-100 hover:text-text-primary'
              }`
            }
          >
            <Icon className="w-4 h-4 flex-shrink-0" />
            <span className="truncate">{label}</span>
          </NavLink>
        ))}
      </nav>

      <div className="p-4 border-t border-border">
        <div className="text-xs text-text-muted">
          Phase 1 · Foundation
        </div>
      </div>
    </aside>
  )
}
