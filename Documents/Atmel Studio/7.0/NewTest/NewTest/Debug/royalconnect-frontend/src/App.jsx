import React, { useState } from "react";
import { Home, ShieldCheck, TrendingUp, Target, FileText, Bell, Car, Search, Menu, X, Phone, ChevronDown, HelpCircle, ArrowRight, UserRound } from "lucide-react";

const COLORS = {
  ink: "#16202A",
  gold: "#C8A96B",
};

function Logo() {
  return (
    <div className="brand" aria-label="Royal Square Financial">
      <div className="brand-mark">R</div>
      <div className="brand-copy">
        <div className="brand-name">ROYAL SQUARE</div>
        <div className="brand-sub">FINANCIAL</div>
      </div>
    </div>
  );
}

function Header({ onLogin }) {
  const [openMenu, setOpenMenu] = useState(null);
  const [mobileOpen, setMobileOpen] = useState(false);

  const menuData = {
    Individuals: [
      ["My Financial Position", "See your assets, liabilities and net worth."],
      ["Insurance", "Manage policies and report claims."],
      ["Investments", "View your investment portfolio."],
      ["Financial Planning", "Work toward your personal goals."],
      ["Goals", "Track the goals created with your adviser."],
      ["Documents", "Find important documents in one place."],
    ],
    Business: [
      ["Business Assurance", "Protect the people and value that matter."],
      ["Business Insurance", "Manage cover for your business."],
      ["Business Planning", "Organise your business financial needs."],
      ["Business Documents", "Access your business records."],
    ],
    Services: [
      ["Request a Service", "Ask Royal Square to help with a task."],
      ["Change Details", "Update your address or banking information."],
      ["Request Documents", "Get policy and investment documents."],
      ["Book a Consultation", "Speak to your adviser or Royal Square."],
    ],
    Claims: [
      ["Report an Accident", "Get guided through the accident process."],
      ["My Claims", "Track existing claims."],
      ["What to Collect", "See the evidence you may need."],
    ],
  };

  return (
    <header className="site-header">
      <div className="top-header">
        <Logo />

        <div className="desktop-actions">
          <button className="text-action">
            <span>🇿🇦</span> South Africa <ChevronDown size={15} />
          </button>
          <button className="text-action"><Phone size={17} /> Contact</button>
          <button className="text-action"><Search size={17} /> Search</button>
          <button className="login-top" onClick={onLogin}><UserRound size={17} /> Login</button>
        </div>

        <button
          className="mobile-menu-button"
          onClick={() => setMobileOpen(!mobileOpen)}
          aria-label="Open menu"
        >
          {mobileOpen ? <X /> : <Menu />}
        </button>
      </div>

      <nav className={`main-nav ${mobileOpen ? "mobile-open" : ""}`}>
        {Object.keys(menuData).map((name) => (
          <div className="nav-item" key={name}>
            <button
              className="nav-button"
              onClick={() => setOpenMenu(openMenu === name ? null : name)}
            >
              {name} <ChevronDown size={15} />
            </button>

            {openMenu === name && (
              <div className="mega-menu">
                <div className="mega-menu-intro">
                  <span className="eyebrow">ROYALCONNECT</span>
                  <h3>{name}</h3>
                  <p>Explore the services available through one secure Royal Square experience.</p>
                </div>
                <div className="mega-menu-grid">
                  {menuData[name].map(([title, text]) => (
                    <button className="menu-card" key={title}>
                      <strong>{title}</strong>
                      <span>{text}</span>
                      <ArrowRight size={16} />
                    </button>
                  ))}
                </div>
              </div>
            )}
          </div>
        ))}

        <button className="nav-login" onClick={onLogin}>Login</button>
      </nav>
    </header>
  );
}

function HelpBar() {
  return (
    <button className="floating-help">
      <HelpCircle size={20} />
      <span>I NEED HELP</span>
    </button>
  );
}

function Home({ onLogin }) {
  return (
    <main>
      <section className="hero">
        <div className="hero-left">
          <span className="eyebrow">ROYAL SQUARE FINANCIAL</span>
          <h1>Your financial life.<br /><em>One secure place.</em></h1>
          <p>
            RoyalConnect brings your insurance, investments, documents,
            goals, reminders and service requests together in one simple
            experience.
          </p>

          <div className="hero-actions">
            <button className="btn-primary" onClick={onLogin}>Login to RoyalConnect</button>
            <button className="btn-secondary">Explore the platform <ArrowRight size={18} /></button>
          </div>

          <div className="trust-row">
            <div><ShieldCheck size={18} /><span>Secure by design</span></div>
            <div><UserRound size={18} /><span>Human support when you need it</span></div>
          </div>
        </div>

        <div className="hero-tree-panel">
          <div className="tree-glow" />
          <div className="tree">
            <div className="tree-roots">
              <span>Financial Planning</span>
              <span>Protection</span>
              <span>Growth</span>
            </div>
            <div className="tree-trunk">ROYAL<br />SQUARE</div>
            <div className="tree-branch branch-one">Insurance</div>
            <div className="tree-branch branch-two">Investments</div>
            <div className="tree-branch branch-three">Goals</div>
            <div className="tree-branch branch-four">Services</div>
          </div>
          <div className="tree-caption">One platform connecting the parts of your financial life.</div>
        </div>
      </section>

      <section className="section light">
        <div className="section-heading">
          <span className="eyebrow">ONE EXPERIENCE</span>
          <h2>What can we help you with?</h2>
          <p>Simple on the surface. Powerful behind the scenes.</p>
        </div>

        <div className="service-grid">
          <FeatureCard icon={<ShieldCheck />} title="Insurance" text="Manage cover, documents and claims without unnecessary paperwork." />
          <FeatureCard icon={<TrendingUp />} title="Investments" text="See your investment position and connect it to your bigger financial picture." />
          <FeatureCard icon={<Target />} title="Goals" text="Track the milestones that you and your adviser are working toward." />
          <FeatureCard icon={<Bell />} title="Reminders" text="Stay ahead of renewals, reviews, documents and important dates." />
          <FeatureCard icon={<FileText />} title="Documents" text="Find important financial documents securely in one place." />
          <FeatureCard icon={<Car />} title="Report an Accident" text="Follow step-by-step guidance from the scene to claim submission." />
        </div>
      </section>

      <section className="section split-section">
        <div>
          <span className="eyebrow">DESIGNED FOR EVERYONE</span>
          <h2>Technology should guide you — not confuse you.</h2>
          <p>
            Choose an easy guided experience with clear instructions, large
            buttons, language support and a direct way to contact Royal Square
            when you need a person.
          </p>
          <button className="btn-secondary">See how guided mode works <ArrowRight size={18} /></button>
        </div>
        <div className="guided-card">
          <div className="guided-top">
            <span>GUIDED MODE</span>
            <span className="status-dot">●</span>
          </div>
          <h3>Let's do this together.</h3>
          <p>First, tell us what you need help with.</p>
          <button className="guided-option"><span>🚗</span> I had an accident <ArrowRight size={17} /></button>
          <button className="guided-option"><span>📄</span> I need a document <ArrowRight size={17} /></button>
          <button className="guided-option"><span>📞</span> I want to speak to someone <ArrowRight size={17} /></button>
        </div>
      </section>

      <section className="cta-section">
        <div>
          <span className="eyebrow">ROYALCONNECT</span>
          <h2>Less paperwork. Less waiting. More clarity.</h2>
        </div>
        <button className="btn-light" onClick={onLogin}>Enter RoyalConnect <ArrowRight size={18} /></button>
      </section>
    </main>
  );
}

function FeatureCard({ icon, title, text }) {
  return (
    <div className="feature-card">
      <div className="feature-icon">{icon}</div>
      <h3>{title}</h3>
      <p>{text}</p>
      <button>Learn more <ArrowRight size={16} /></button>
    </div>
  );
}

function Login({ onBack, onDashboard }) {
  const [guided, setGuided] = useState(true);

  return (
    <main className="auth-page">
      <div className="auth-left">
        <button className="back-button" onClick={onBack}>← Back to website</button>
        <div className="auth-copy">
          <span className="eyebrow">SECURE ACCESS</span>
          <h1>Welcome back.</h1>
          <p>Sign in to manage your financial life through RoyalConnect.</p>

          <div className="security-note">
            <ShieldCheck size={21} />
            <div>
              <strong>Your security matters</strong>
              <span>Sensitive actions can require an additional verification step.</span>
            </div>
          </div>
        </div>
      </div>

      <div className="auth-panel">
        <div className="login-card">
          <div className="mobile-logo"><Logo /></div>
          <h2>Login</h2>
          <p className="muted">Enter your details to continue.</p>

          <label>Email address</label>
          <input type="email" placeholder="you@example.com" />

          <label>Password</label>
          <input type="password" placeholder="Enter your password" />

          <div className="login-row">
            <label className="check-label"><input type="checkbox" /> Remember me</label>
            <button className="link-button">Forgot password?</button>
          </div>

          <button className="btn-primary full" onClick={onDashboard}>Login</button>

          <div className="guided-toggle">
            <div>
              <strong>Guided Mode</strong>
              <span>Make the app simpler and more step-by-step.</span>
            </div>
            <button
              className={`switch ${guided ? "on" : ""}`}
              onClick={() => setGuided(!guided)}
              aria-label="Toggle guided mode"
            >
              <span />
            </button>
          </div>

          <button className="biometric-demo">
            <div className="bio-circle">◉</div>
            <div>
              <strong>Use device biometrics</strong>
              <span>Face or fingerprint for supported sensitive actions</span>
            </div>
          </button>

          <p className="login-help">
            Need help? <button className="link-button">Speak to Royal Square</button>
          </p>
        </div>
      </div>
    </main>
  );
}

function Dashboard() {
  return (
    <main className="dashboard-page">
      <div className="dashboard-welcome">
        <div>
          <span className="eyebrow">MY ROYALCONNECT</span>
          <h1>Good evening.</h1>
          <p>Here is a simple view of your financial life.</p>
        </div>
        <button className="btn-primary"><HelpCircle size={18} /> I NEED HELP</button>
      </div>

      <section className="networth">
        <div>
          <span className="label">TOTAL NET WORTH</span>
          <div className="amount">R1,245,000</div>
          <span className="positive">↑ 4.8% this year</span>
        </div>
        <div className="nw-details">
          <div><span>Assets</span><strong>R1,650,000</strong></div>
          <div><span>Liabilities</span><strong>R405,000</strong></div>
        </div>
      </section>

      <div className="dashboard-layout">
        <section>
          <div className="section-heading compact">
            <span className="eyebrow">YOUR OVERVIEW</span>
            <h2>What would you like to do?</h2>
          </div>
          <div className="dashboard-grid">
            <DashCard icon={<ShieldCheck />} title="My Insurance" text="Policies, cover and claims" />
            <DashCard icon={<TrendingUp />} title="My Investments" text="Portfolio and statements" />
            <DashCard icon={<Target />} title="My Goals" text="Track your progress" />
            <DashCard icon={<Bell />} title="Reminders" text="3 things need your attention" alert />
            <DashCard icon={<FileText />} title="My Documents" text="Your secure document vault" />
            <DashCard icon={<Car />} title="Report an Accident" text="Guided claim reporting" />
          </div>
        </section>

        <aside className="dashboard-sidebar">
          <div className="side-card">
            <div className="side-card-title"><Bell size={18} /> Upcoming</div>
            <div className="reminder-item"><span>Annual financial review</span><strong>12 Sep</strong></div>
            <div className="reminder-item"><span>Valuation certificate</span><strong>20 Sep</strong></div>
            <div className="reminder-item"><span>Driving licence</span><strong>02 Oct</strong></div>
          </div>

          <div className="side-card">
            <div className="side-card-title"><Target size={18} /> Your goals</div>
            <div className="goal-mini"><div className="goal-line"><span>House deposit</span><strong>60%</strong></div><div className="progress"><span style={{width:"60%"}} /></div></div>
            <div className="goal-mini"><div className="goal-line"><span>Retirement</span><strong>38%</strong></div><div className="progress"><span style={{width:"38%"}} /></div></div>
          </div>
        </aside>
      </div>
    </main>
  );
}

function DashCard({ icon, title, text, alert }) {
  return (
    <button className="dash-card">
      <div className="dash-icon">{icon}</div>
      <div className="dash-body">
        <h3>{title}</h3>
        <p>{text}</p>
      </div>
      {alert ? <span className="alert-badge">3</span> : <ArrowRight className="dash-arrow" size={18} />}
    </button>
  );
}

export default function App() {
  const [page, setPage] = useState("home");

  const goLogin = () => setPage("login");
  const goHome = () => setPage("home");
  const goDashboard = () => setPage("dashboard");

  return (
    <>
      {page !== "login" && <Header onLogin={goLogin} />}
      {page === "home" && <Home onLogin={goLogin} />}
      {page === "login" && <Login onBack={goHome} onDashboard={goDashboard} />}
      {page === "dashboard" && <Dashboard />}
      {page !== "login" && <HelpBar />}
    </>
  );
}