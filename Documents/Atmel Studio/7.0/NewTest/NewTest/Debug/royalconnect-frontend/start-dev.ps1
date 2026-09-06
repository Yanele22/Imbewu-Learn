$ErrorActionPreference = 'Stop'

$nodeCandidates = @(
  "$env:ProgramFiles\nodejs\node.exe",
  "$env:ProgramFiles\Node.js\node.exe",
  "$env:LOCALAPPDATA\Programs\nodejs\node.exe",
  "C:\\Program Files\\nodejs\\node.exe"
)

$nodePath = $nodeCandidates | Where-Object { $_ -and (Test-Path $_) } | Select-Object -First 1

if (-not $nodePath) {
  $resolved = Get-Command node -ErrorAction SilentlyContinue
  if ($resolved) {
    $nodePath = $resolved.Source
  }
}

if (-not $nodePath) {
  Write-Error "Node.js was not found. Please install the LTS version from https://nodejs.org/ and try again."
  exit 1
}

$nodeDir = Split-Path $nodePath -Parent
$env:Path = "$nodeDir;$env:Path"
Set-Location $PSScriptRoot

Write-Host "Using Node.js at: $nodePath"
Write-Host "Installing dependencies..."
npm install
Write-Host "Starting Vite dev server..."
npm run dev -- --host 0.0.0.0
