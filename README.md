# Tilt Five First Light - Native C++ Application

Minimal native C++ application for Tilt Five AR glasses using the NDK (no Unity).

## Environment

**Tilt Five Software:**
- Driver version: `[FILL IN]`
- NDK version: `[FILL IN]`
- NDK install path: `[FILL IN]`

**Build Tools:**
- CMake: `[FILL IN]`
- Ninja: `[FILL IN]`
- MSVC: `[FILL IN]`
- Windows SDK: `[FILL IN]`

**Graphics:**
- OpenGL version: `[FILL IN]`

## Setup

1. Install Tilt Five Driver and verify demo runs
2. Install Tilt Five Native C SDK (NDK)
3. Copy NDK files to `third_party/tiltfive_ndk`:
   ```
   third_party/tiltfive_ndk/
   ├── include/        (copy headers from NDK install)
   └── lib/            (copy .lib and .dll from NDK install)
   ```

## Build

```bash
cmake -B build -G Ninja
cmake --build build
```

## Run

```bash
build/TiltFiveApp.exe
```

## Status

Current milestone: Setting up baseline HelloT5Cube example
