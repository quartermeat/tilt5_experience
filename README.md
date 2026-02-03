# Tilt Five 3D Pong — Native C++ Game

A tabletop 3D Pong game for Tilt Five AR glasses using the Native SDK (no Unity).

## Project Structure

```
├── src/                    # Application source code
│   └── main.cpp            # Main application entry point
├── glad/                   # Local: GLAD OpenGL loader (generated, ignored)
│   └── OpenGL_4_6_Core/    # OpenGL 4.6 Core profile
├── glfw-3.3.8.bin.WIN64/   # Local: GLFW pre-built binaries (ignored)
├── glm/                    # Local: GLM math library (ignored)
├── glwrapper/              # OpenGL wrapper classes (committed)
├── glapplication/          # Application framework (committed)
├── t5wrapper/              # Tilt Five SDK wrapper classes (committed)
├── third_party/
│   └── TiltFive/           # Local: Tilt Five NDK headers + libs (ignored)
├── util/                   # Utility classes (committed)
├── CMakeLists.txt          # Build configuration
└── claude.md               # Project plan and milestones
```

## Dependencies

These dependencies are intentionally not committed. Each developer should install them locally.
If any are missing, the build will fail.

## Setup Checklist

- Install Tilt Five Driver
- Install Tilt Five NDK and copy into `third_party/TiltFive/`
- Download and extract GLFW into `glfw-3.3.8.bin.WIN64/`
- Generate and extract GLAD into `glad/OpenGL_4_6_Core/`
- Download and extract GLM into `glm/`

Optional: run the setup helper to download/generate dependencies (except the Tilt Five NDK):

```powershell
./scripts/setup.ps1
```

Notes:
- The script downloads GLFW and GLM from their official GitHub releases.
- The script generates GLAD using `python -m glad` (Python required).
- For integrity, pass expected SHA256 hashes:
  - `./scripts/setup.ps1 -GlfwSha256 <hash> -GlmSha256 <hash>`

To validate an existing local setup without downloading:

```powershell
./scripts/verify_deps.ps1
```

### Tilt Five NDK
1. Install the Tilt Five Driver
   - `https://www.tiltfive.com/setup`
2. Download the Native Development Kit (NDK) from the Tilt Five developer portal
3. Copy the SDK into `third_party/TiltFive/` with this layout:
   - `third_party/TiltFive/include/` — SDK headers
   - `third_party/TiltFive/lib/win/x86_64/` — Windows x64 libraries (.lib, .dll)

### GLFW (pre-built)
Download from:
`https://www.glfw.org/download.html`
- Version: 3.3.8 (Windows 64-bit binaries)
- Extract to `glfw-3.3.8.bin.WIN64/`

### GLAD (OpenGL loader)
Generate at:
`https://glad.dav1d.de/`
- Language: C/C++
- Specification: OpenGL
- Profile: Core
- API gl: Version 4.6
- Generate a loader: Yes
- Extract to `glad/OpenGL_4_6_Core/`

### GLM (math library)
Download from:
`https://github.com/g-truc/glm/releases`
- Header-only, extract to `glm/`

### Local scaffolding (project-specific)
These folders are committed to this repo and should not be removed:
- `glwrapper/`
- `glapplication/`
- `t5wrapper/`
- `util/`

## Licensing and provenance
- `glwrapper/` — see `glwrapper/LICENSE.txt`
- `glapplication/` — see `glapplication/LICENSE.txt`
- `t5wrapper/` — see `t5wrapper/LICENSE.txt`
- `Tilt Five NDK`, `GLFW`, `GLAD`, and `GLM` are third-party dependencies.
  Keep their license files with your local copies and review their terms.

## Build

```bash
# Configure (first time)
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build
cmake --build build --config Release
```

## Run

```bash
build/Release/TiltFiveApp.exe
```

Requires:
- Tilt Five glasses connected and recognized by driver
- Tilt Five service running

## Current Status

**Milestone 1 — Court rendering** ✓
- Wireframe court bounds rendered (0.40m × 0.25m × 0.60m)
- Fixed in world space above gameboard

**Milestone 2 — Paddle control** ✓
- Read wand pose
- Map to paddle position

See [claude.md](claude.md) for full milestone plan.
