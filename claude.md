# claude.md — Tilt Five NDK (C/C++) first-light in VS Code

## Goal
Get Tilt Five hardware working and run a minimal native C++ app (no Unity) that:
1) connects to Tilt Five Service
2) reserves glasses
3) receives head pose updates
4) submits stereo frames (left/right) continuously

## Preconditions (must pass before coding)
- Install latest Tilt Five Driver; verify a Tilt Five demo runs
- Install Tilt Five Native C SDK (NDK)

## Repo plan
- Use CMake + Ninja + MSVC (Windows)
- Keep the first milestone OpenGL (simplest to validate)

Structure:
- /src
- /third_party/tiltfive_ndk  (headers + libs copied from installed SDK)
- CMakeLists.txt
- README.md (record versions: driver, NDK, compiler)

## First-light strategy
1) Start from a known minimal NDK example (HelloT5Cube) and get it building unchanged.
2) Once it runs, only then begin modifications:
   - add robust logging around service connect / reserve glasses / pose read
   - simplify rendering (colored quad per eye) if necessary
   - add wand pose printing (optional)

## Non-negotiables
- Do not add extra dependencies unless build is blocked.
- Fix one error at a time; always paste the exact build/runtime error text.
- If demo works but app doesn’t: assume linking/runtimes first, then API usage, then rendering.

## Done criteria
- `cmake --build` succeeds in VS Code terminal
- running the exe shows stable pose updates and stereo imagery in the glasses
