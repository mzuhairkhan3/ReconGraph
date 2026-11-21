import { useMemo, useRef, useState } from 'react'
import { Canvas, useFrame } from '@react-three/fiber'
import { OrbitControls, Text, Html } from '@react-three/drei'
import * as THREE from 'three'
import { GraphNodeData, NODE_COLORS } from '../../types'

interface Props {
  nodes: GraphNodeData[]
}

function NodeMesh({
  node,
  position,
  selected,
  onSelect,
}: {
  node: GraphNodeData
  position: [number, number, number]
  selected: boolean
  onSelect: () => void
}) {
  const color = NODE_COLORS[node.type] || NODE_COLORS.unknown
  const meshRef = useRef<THREE.Mesh>(null)

  useFrame(() => {
    if (meshRef.current && selected) {
      meshRef.current.rotation.y += 0.01
    }
  })

  return (
    <group position={position}>
      <mesh ref={meshRef} onClick={onSelect}>
        <sphereGeometry args={[selected ? 0.35 : 0.25, 16, 16]} />
        <meshStandardMaterial
          color={color}
          emissive={color}
          emissiveIntensity={selected ? 0.45 : 0.15}
          roughness={0.4}
          metalness={0.2}
        />
      </mesh>
      {selected && (
        <Html distanceFactor={8} position={[0, 0.55, 0]} center>
          <div className="bg-surface-100 border border-border rounded px-2 py-1 text-[10px] text-text-primary whitespace-nowrap shadow-lg">
            <div className="font-medium">{node.type}</div>
            <div className="text-text-muted max-w-[180px] truncate">{node.url}</div>
          </div>
        </Html>
      )}
    </group>
  )
}

function Scene({ nodes }: Props) {
  const [selectedId, setSelectedId] = useState<number | null>(null)

  // Simple radial / spiral layout for Phase 4
  const positions = useMemo(() => {
    const map = new Map<number, [number, number, number]>()
    const n = nodes.length || 1
    nodes.forEach((node, i) => {
      const angle = (i / n) * Math.PI * 2
      const radius = 2 + (i % 5) * 0.6
      const y = (i % 7) * 0.35 - 1.2
      map.set(node.id, [Math.cos(angle) * radius, y, Math.sin(angle) * radius])
    })
    return map
  }, [nodes])

  return (
    <>
      <ambientLight intensity={0.4} />
      <pointLight position={[10, 10, 10]} intensity={0.8} />
      <pointLight position={[-10, -5, -10]} intensity={0.3} color="#22d3ee" />

      {nodes.map((node) => {
        const pos = positions.get(node.id) || [0, 0, 0]
        return (
          <NodeMesh
            key={node.id}
            node={node}
            position={pos}
            selected={selectedId === node.id}
            onSelect={() => setSelectedId(node.id === selectedId ? null : node.id)}
          />
        )
      })}

      <OrbitControls makeDefault enableDamping dampingFactor={0.1} />
      <gridHelper args={[20, 20, '#1e293b', '#111827']} position={[0, -2.5, 0]} />
    </>
  )
}

export function AttackSurfaceCanvas({ nodes }: Props) {
  if (!nodes.length) {
    return (
      <div className="h-full flex items-center justify-center text-text-muted text-sm">
        No graph data. Run a scan first.
      </div>
    )
  }

  return (
    <Canvas camera={{ position: [0, 2, 8], fov: 50 }} className="rounded-lg">
      <color attach="background" args={['#0a0e17']} />
      <Scene nodes={nodes} />
    </Canvas>
  )
}
