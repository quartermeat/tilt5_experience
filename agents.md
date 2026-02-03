#Message from Quartermeat:
#For full transparency, this is the instructions right now for Codex
#The requirements for this file:
#Codex shall modify this file
#Codex shall not modify claude.md


# Project: Tilt Five Native Game — 3D Pong

## High-level goal
Build and ship a **small, free Tilt Five native C++ game** (no Unity) using the official Native SDK.

The first shipped project will be:
> A tabletop **3D Pong / Ping Pong game** where the ball moves in X/Y/Z and the wand controls a paddle.

This is a learning + shipping project, not an engine project.

---

## Current status
- Native Tilt Five SDK is installed
- A working CMake project exists (ported from HelloTilt5Cube)
- Rendering works
- Head pose works
- A floating object renders correctly and moves with perspective

This means:
> Rendering pipeline + service integration are already functional.

---

## Project constraints
- C++
- Native Tilt Five SDK (no Unity, no Unreal)
- Keep code small and understandable
- Avoid overengineering
- Prefer simple math and explicit code over abstractions
- Always preserve a runnable build

---

## Architecture direction (lightweight)

Core modules (conceptual, not strict file structure):
- **App / Main Loop**
  - Owns update + render loop
  - Calls Game update
- **GameState**
  - Ball position / velocity
  - Paddle positions
  - Score
- **Input**
  - Maps wand pose → player paddle position
- **Simulation**
  - Fixed timestep update (ball movement + collisions)
- **Rendering**
  - Draw court
  - Draw paddles
  - Draw ball
  - Draw minimal UI (score)

Avoid frameworks, ECS, or engine patterns.

---

## Coordinate system (must remain consistent)
Define a stable game space:

Court space:
- X = left / right
- Y = up / down
- Z = toward / away (depth)

Court anchored to board:
- Court origin = center of playfield above board
- Court dimensions (initial target):
  - Width (X): ~0.40m
  - Height (Y): ~0.25m
  - Depth (Z): ~0.60m

Paddle planes:
- Player paddle at `z = -depth/2`
- Opponent paddle at `z = +depth/2`

Everything (ball, paddles, collision, rendering) happens in this space.

---

## Game concept: 3D Pong

Core mechanics:
- One player (wand-controlled paddle)
- One AI paddle
- Ball moves in full 3D
- Ball bounces off:
  - side walls (X)
  - top/bottom walls (Y)
  - paddles (Z planes)
- If ball crosses behind paddle → score

Controls:
- Wand position → paddle X/Y
- Paddle Z is fixed
- Clamp paddle within court bounds

Collision behavior:
- On paddle hit:
  - Reverse Z velocity
  - Add deflection:
    - vel.x += (ball.x - paddle.x) * factor
    - vel.y += (ball.y - paddle.y) * factor
  - Normalize velocity to target speed
  - Slight speed increase over time

AI:
- Simple target-following AI
- Tracks predicted intercept point
- Has capped movement speed so it's beatable

---

## Milestone progression

### Milestone 1 — Deterministic court
- Create court coordinate frame
- Render visible court bounds (wireframe box or grid)
- Verify objects are stable in space when head moves

### Milestone 2 — Paddle control
- Read wand pose
- Map wand X/Y into court space
- Render paddle at mapped position

### Milestone 3 — Ball simulation
- Add ball position + velocity
- Move ball every frame or via fixed timestep
- Render ball

### Milestone 4 — Collisions
- Wall bounces (X/Y)
- Paddle collision (Z planes)
- Ball resets when a point is scored

### Milestone 5 — Playable loop
- Score tracking
- Simple AI opponent
- Continuous play loop

### Milestone 6 — Polish (only after playable)
- Ball trail or glow for depth readability
- Court visual clarity
- Sound effects (optional)
- Subtle UI improvements

---

## Visual priorities (important for depth perception)
At least one of:
- Court wireframe/grid
- Ball trail
- Floor shadow plane
- Depth tick marks

Without depth cues, 3D motion becomes hard to judge.

---

## Guardrails for the agent (Claude Code)
When working on this project:

- Do not refactor working systems unless asked
- Do not introduce architecture patterns (ECS, etc.)
- Do not add external dependencies unless strictly necessary
- Prefer modifying existing files over creating many new ones
- Keep changes minimal and reversible
- If uncertain, ask rather than guessing

Focus is always:
> Small working steps that preserve buildability.

---

## Definition of success
This project is successful when:
- App builds cleanly
- The game runs in Tilt Five glasses
- Player can control paddle with wand
- Ball moves in full 3D
- A simple playable loop exists
- It could reasonably be shared as a free Tilt Five experience

Not success:
- Complex engine
- Feature creep
- Over-ambitious scope

---

## Shared instructions for Claude
Claude should maintain `claude.md` with the same high-level project plan and guardrails described in this file.
Add or update any workflow requirements for Claude there (branching, PRs, testing) to match the intent here.
Keep `claude.md` aligned with these constraints and milestones.
