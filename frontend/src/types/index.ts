export type NodeType =
  | 'root'
  | 'page'
  | 'api_endpoint'
  | 'form'
  | 'parameter'
  | 'auth_point'
  | 'admin_route'
  | 'external_link'
  | 'static_resource'
  | 'unknown';

export interface GraphNodeData {
  id: number;
  url: string;
  type: NodeType | string;
  method: string;
}

export const NODE_COLORS: Record<string, string> = {
  root: '#22d3ee',
  page: '#94a3b8',
  api_endpoint: '#34d399',
  form: '#fbbf24',
  parameter: '#a78bfa',
  auth_point: '#f87171',
  admin_route: '#fb7185',
  external_link: '#64748b',
  static_resource: '#475569',
  unknown: '#64748b',
};
