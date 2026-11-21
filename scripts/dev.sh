#!/usr/bin/env bash
# ReconGraph development helper
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "==> ReconGraph dev helper"
echo "Root: $ROOT"

case "${1:-}" in
  backend)
    cd "$ROOT/backend"
    mkdir -p build && cd build
    cmake ..
    cmake --build .
    echo "Starting backend on :8080 ..."
    ./recongraph_server
    ;;
  frontend)
    cd "$ROOT/frontend"
    if [ ! -d node_modules ]; then
      npm install
    fi
    npm run dev
    ;;
  build-backend)
    cd "$ROOT/backend"
    mkdir -p build && cd build
    cmake ..
    cmake --build .
    echo "Backend binary: $ROOT/backend/build/recongraph_server"
    ;;
  *)
    echo "Usage: $0 {backend|frontend|build-backend}"
    echo "  backend        – configure, build, and run the C++ server"
    echo "  frontend       – install deps if needed and start Vite"
    echo "  build-backend  – configure and build only"
    exit 1
    ;;
esac
