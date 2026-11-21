export function AttackSurfaceMap() {
  return (
    <div className="h-full flex flex-col space-y-4">
      <div>
        <h1 className="text-2xl font-semibold tracking-tight">Attack Surface Map</h1>
        <p className="mt-1 text-text-secondary text-sm">
          Interactive 3D visualization of discovered resources and relationships
        </p>
      </div>

      <div className="flex-1 min-h-[500px] card flex items-center justify-center border-dashed">
        <div className="text-center space-y-2">
          <div className="text-text-muted text-sm">3D Graph Viewport</div>
          <div className="text-xs text-text-muted max-w-sm">
            React Three Fiber visualization will be implemented in Phase 4.
            Nodes will represent pages, endpoints, forms, parameters, and auth points.
          </div>
        </div>
      </div>
    </div>
  )
}
