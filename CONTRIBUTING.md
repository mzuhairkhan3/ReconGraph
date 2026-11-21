# Contributing to ReconGraph

Thank you for your interest.

ReconGraph is a structured, multi-phase cybersecurity portfolio project. The core architecture (Phases 1–6) is in place. Contributions that improve correctness, safety, documentation, or visualization quality are welcome.

## Principles

- Prefer clear, modular C++ and TypeScript.
- Custom DSA must remain meaningfully used, not ornamental.
- Security and scope enforcement are non-negotiable.
- Do **not** add stealth scanning, credential theft, destructive exploits, or unauthorized access capabilities.
- Keep the UI dark, professional, and Martel-based.

## Development Setup

See [docs/user_guide/DEMO.md](docs/user_guide/DEMO.md) for full run instructions.

Quick path:

```bash
# Backend
cd backend && mkdir -p build && cd build && cmake .. && cmake --build . && ./recongraph_server

# Frontend (new terminal)
cd frontend && npm install && npm run dev
```

Or use the helper:

```bash
./scripts/dev.sh backend
./scripts/dev.sh frontend
```

## Pull Requests

1. Fork and branch from `main`
2. Keep changes focused
3. Ensure backend compiles and frontend builds
4. Describe *what* and *why* in the PR body
5. Note any remaining limitations honestly

## Reporting Issues

Open an issue with expected vs actual behaviour and steps to reproduce. Lab-only targets preferred in examples.

---

Built for authorized security testing and education.
