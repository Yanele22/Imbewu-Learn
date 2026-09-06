# Royal Square Financial — Digital Client Platform (prototype)

A single-file, no-build-step prototype for the AfriHack pitch. Everything —
markup, styles, and behaviour — lives in `index.html` so it can be opened
directly in a browser with no install step (Live Server in VS Code works
well for auto-reload while editing).

## How to run it

1. Open this folder in VS Code.
2. Install the "Live Server" extension (if you don't have it), right-click
   `index.html`, and choose "Open with Live Server".
   — or just double-click `index.html` to open it straight in a browser.

## What's built so far

- **RoyalOne entry experience** — a responsive, client-first landing screen
  that positions Royal Square as one secure home for money, protection,
  wealth and advice.
- **Login** — asks whether you're a **Client** or **Royal Square Personnel**
  before continuing.
- **Client home** — a life-organised nav grid (My Money, My Insurance,
  My Investments, My Property, My Family, My Documents, I Need Help,
  Speak to an Adviser), each opening its own panel.
- **I Need Help → Vehicle accident → Accident Mode** — the fully built-out
  flow: safety check → guided photo/location/description capture →
  simulated claim-building → review → submission with a reference number.
  Other incident types are stubbed with a short "not wired up yet" note.
- **Royal Square Personnel** — logs into a placeholder staff dashboard
  (clients under management, tasks due, open claims, new leads). This side
  hasn't been built out to match the client side yet.

## Known gaps / next steps

- All data is hardcoded in `index.html` (see the top of the `<script>`
  block area and the dashboard markup) — there's no backend, database, or
  real authentication.
- The "3D" is CSS parallax, not true rotatable WebGL geometry.
- Property damage / financial problem / family emergency / document
  problem incident types aren't built out.
- The Royal Square Personnel portal needs the "see this client's world"
  view described in the platform brief (profile completeness, active
  claims, pending requests, per-client drill-down).
- No AI assistant, multi-language/voice, or real provider API integration
  yet — these are pitch-deck "big vision" items, not current code.

## Source material

Built from Royal Square Financial's own business brief (client dashboard,
automated reminders, goal tracking, and a detailed short-term-insurance
claims workflow) plus a follow-up platform vision doc reframing it as a
single secure client platform organised around the client's life rather
than the company's internal departments.
