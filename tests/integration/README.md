# Integration Tests

## Manual Integration Checklist (Phase 6)

Use this checklist when verifying a fresh build:

### Backend

- [ ] `cmake .. && cmake --build .` succeeds
- [ ] `./recongraph_server` starts without crash
- [ ] `GET /api/health` returns JSON with status `ok`
- [ ] `POST /api/normalize` with a valid URL returns canonical form
- [ ] `POST /api/scan` against `http://127.0.0.1:PORT` completes and returns node/edge counts
- [ ] `GET /api/graph` returns nodes after a scan
- [ ] `GET /api/stats` reflects the last scan

### Frontend

- [ ] `npm install && npm run dev` starts
- [ ] Dashboard shows backend health
- [ ] New Recon form can submit a scan and display results
- [ ] Attack Surface Map renders nodes (after a scan)
- [ ] Endpoints table lists discovered routes
- [ ] Analysis page shows ranked items
- [ ] Reports page can download a JSON file

### Cross-cutting

- [ ] Vite proxy forwards `/api/*` to port 8080
- [ ] CORS does not block browser requests in development
- [ ] Cancelling / limiting depth actually bounds the crawl

Automated test harnesses can be added later using a C++ test framework (Catch2 / GoogleTest) and Playwright/Cypress for the UI.
