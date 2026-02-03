$ErrorActionPreference = "Stop"

param(
    [switch]$SkipGlfw,
    [switch]$SkipGlm,
    [switch]$SkipGlad,
    [switch]$SkipTiltFive
)

$RepoRoot = Split-Path -Parent $PSScriptRoot

function Ensure-Dir($Path) {
    if (-not (Test-Path $Path)) {
        New-Item -ItemType Directory -Force -Path $Path | Out-Null
    }
}

function Download-And-ExtractZip($Url, $DestinationDir) {
    $tmp = Join-Path $env:TEMP ([System.IO.Path]::GetRandomFileName() + ".zip")
    Write-Host "Downloading $Url"
    Invoke-WebRequest -Uri $Url -OutFile $tmp
    Ensure-Dir $DestinationDir
    Expand-Archive -Path $tmp -DestinationPath $DestinationDir -Force
    Remove-Item $tmp -Force
}

function Ensure-Glfw {
    $glfwDir = Join-Path $RepoRoot "glfw-3.3.8.bin.WIN64"
    if (Test-Path $glfwDir) {
        Write-Host "GLFW already present: $glfwDir"
        return
    }

    $url = "https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip"
    Download-And-ExtractZip $url $RepoRoot
}

function Ensure-Glm {
    $glmDir = Join-Path $RepoRoot "glm"
    if (Test-Path $glmDir) {
        Write-Host "GLM already present: $glmDir"
        return
    }

    $url = "https://github.com/g-truc/glm/releases/download/0.9.9.9/glm-0.9.9.9.zip"
    Download-And-ExtractZip $url $RepoRoot

    # The archive extracts to glm/ by default; keep that folder name.
}

function Ensure-Glad {
    $gladDir = Join-Path $RepoRoot "glad\\OpenGL_4_6_Core"
    if (Test-Path $gladDir) {
        Write-Host "GLAD already present: $gladDir"
        return
    }

    $python = Get-Command python -ErrorAction SilentlyContinue
    if (-not $python) {
        $python = Get-Command py -ErrorAction SilentlyContinue
    }
    if (-not $python) {
        Write-Error "Python is required to generate GLAD. Install Python and rerun scripts/setup.ps1."
    }

    Write-Host "Generating GLAD (OpenGL 4.6 Core) via python -m glad"
    & $python.Source -m pip install glad
    Ensure-Dir $gladDir
    & $python.Source -m glad --profile core --api gl=4.6 --generator c --spec gl --out-path $gladDir
}

function Ensure-TiltFiveNdk {
    $t5Dir = Join-Path $RepoRoot "third_party\\TiltFive"
    if (Test-Path $t5Dir) {
        Write-Host "Tilt Five NDK already present: $t5Dir"
        return
    }

    Write-Error "Tilt Five NDK not found. Install from the Tilt Five developer portal and copy into third_party\\TiltFive."
}

Write-Host "=== Tilt Five dependency setup ==="
Write-Host "Repo: $RepoRoot"

if (-not $SkipGlfw) { Ensure-Glfw }
if (-not $SkipGlm) { Ensure-Glm }
if (-not $SkipGlad) { Ensure-Glad }
if (-not $SkipTiltFive) { Ensure-TiltFiveNdk }

Write-Host "Setup complete."
