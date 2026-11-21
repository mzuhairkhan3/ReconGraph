import { BrowserRouter, Routes, Route } from 'react-router-dom'
import { Layout } from './components/layout/Layout'
import { Dashboard } from './pages/Dashboard'
import { NewRecon } from './pages/NewRecon'
import { AttackSurfaceMap } from './pages/AttackSurfaceMap'
import { Endpoints } from './pages/Endpoints'
import { Parameters } from './pages/Parameters'
import { Analysis } from './pages/Analysis'
import { ScanHistory } from './pages/ScanHistory'
import { Reports } from './pages/Reports'
import { Settings } from './pages/Settings'

function App() {
  return (
    <BrowserRouter>
      <Layout>
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/new-recon" element={<NewRecon />} />
          <Route path="/map" element={<AttackSurfaceMap />} />
          <Route path="/endpoints" element={<Endpoints />} />
          <Route path="/parameters" element={<Parameters />} />
          <Route path="/analysis" element={<Analysis />} />
          <Route path="/history" element={<ScanHistory />} />
          <Route path="/reports" element={<Reports />} />
          <Route path="/settings" element={<Settings />} />
        </Routes>
      </Layout>
    </BrowserRouter>
  )
}

export default App
