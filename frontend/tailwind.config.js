/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      fontFamily: {
        martel: ['Martel', 'serif'],
        sans: ['Martel', 'serif'],
      },
      colors: {
        // Dark cybersecurity palette
        surface: {
          DEFAULT: '#0a0e17',
          50: '#0d121f',
          100: '#111827',
          200: '#1a2234',
          300: '#243044',
        },
        border: {
          DEFAULT: '#1e293b',
          subtle: '#1a2332',
        },
        accent: {
          cyan: '#22d3ee',
          green: '#34d399',
          red: '#f87171',
          amber: '#fbbf24',
        },
        text: {
          primary: '#e2e8f0',
          secondary: '#94a3b8',
          muted: '#64748b',
        }
      },
      boxShadow: {
        'glow-cyan': '0 0 20px rgba(34, 211, 238, 0.15)',
        'glow-green': '0 0 20px rgba(52, 211, 153, 0.15)',
      }
    },
  },
  plugins: [],
}
