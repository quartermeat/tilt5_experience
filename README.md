# Tilt Five 3D Pong — Native C++ Game

A tabletop 3D Pong game for Tilt Five AR glasses using the Native SDK (no Unity).

## Project Structure

```
├── src/                    # Application source code
│   └── main.cpp            # Main application entry point
├── glad/                   # GLAD OpenGL loader (generated)
│   └── OpenGL_4_6_Core/    # OpenGL 4.6 Core profile
├── glfw-3.3.8.bin.WIN64/   # GLFW pre-built binaries for Windows
├── glm/                    # GLM math library (header-only)
├── glwrapper/              # OpenGL wrapper classes (Shader, Framebuffer, etc.)
├── glapplication/          # Application framework (Transform, etc.)
├── t5wrapper/              # Tilt Five SDK wrapper classes
├── third_party/
│   └── TiltFive/           # Tilt Five NDK headers and libs
├── util/                   # Utility classes (ChangeDetector, etc.)
├── CMakeLists.txt          # Build configuration
└── claude.md               # Project plan and milestones
```

## Dependencies

### Tilt Five NDK
1. Install Tilt Five Driver from https://www.tiltfive.com/setup
2. Download Native Development Kit (NDK) from Tilt Five developer portal
3. Copy to `third_party/TiltFive/`:
   - `include/` — SDK headers
   - `lib/win/x86_64/` — Windows x64 libraries (.lib, .dll)

### GLFW (pre-built)
Download from https://www.glfw.org/download.html
- Version: 3.3.8 (Windows 64-bit binaries)
- Extract to `glfw-3.3.8.bin.WIN64/`

### GLAD (OpenGL loader)
Generate at https://glad.dav1d.de/
- Language: C/C++
- Specification: OpenGL
- Profile: Core
- API gl: Version 4.6
- Generate a loader: Yes
- Extract to `glad/OpenGL_4_6_Core/`

### GLM (math library)
Download from https://github.com/g-truc/glm/releases
- Header-only, extract to `glm/`

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

**Milestone 2 — Paddle control** (next)
- Read wand pose
- Map to paddle position

See [claude.md](claude.md) for full milestone plan.
