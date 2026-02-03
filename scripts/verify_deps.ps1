$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot

function Assert-Exists($Path, $Label) {
    if (-not (Test-Path $Path)) {
        Write-Error "$Label missing at $Path"
    }
}

Write-Host "=== Verifying local dependencies ==="
Write-Host "Repo: $RepoRoot"

Assert-Exists (Join-Path $RepoRoot "third_party\\TiltFive\\include") "Tilt Five NDK headers"
Assert-Exists (Join-Path $RepoRoot "third_party\\TiltFive\\lib") "Tilt Five NDK libs"

Assert-Exists (Join-Path $RepoRoot "glad\\OpenGL_4_6_Core\\include\\glad\\gl.h") "GLAD headers"
Assert-Exists (Join-Path $RepoRoot "glad\\OpenGL_4_6_Core\\src\\gl.c") "GLAD source"

Assert-Exists (Join-Path $RepoRoot "glfw-3.3.8.bin.WIN64\\include\\GLFW\\glfw3.h") "GLFW headers"
Assert-Exists (Join-Path $RepoRoot "glm\\glm\\glm.hpp") "GLM headers"

Write-Host "All required files are present."
