# 01: Project Scaffold, Raylib C++ & Retro Title Screen

**What to build:** A working C++ application skeleton running Raylib with a hermetic static build and headless unit test runner. When launched, the window renders a pixel-perfect arcade title screen with golden Triforce emblem, retro logo, and blinking "PRESS SPACE TO START" prompt that transitions to the game scene.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `make` builds the standalone desktop binary `bin/zelda_game` without errors.
- [x] `make test` builds and executes headless unit tests via `bin/test_runner` reporting 0 failures.
- [x] Retro pixel title screen renders on 320x240 virtual canvas scaled up with integer nearest-neighbor sampling.
- [x] Golden Triforce symbol, retro title typography, and blinking prompt match original design.
- [x] Pressing `SPACE` or `ENTER` triggers transition to the Overworld scene.

## Resolution Summary

- Vendored Raylib 5.5 static library for Linux x64 and `nlohmann/json` single-header.
- Configured `Makefile` with `g++` (C++20) supporting fast incremental compilation, static linkage, and headless testing.
- Created `src/core/types.hpp` and `src/core/GameConfig.hpp` for decoupled simulation physics and math.
- Implemented `VirtualCanvas` in `src/render/RenderUtils.cpp` maintaining strict 320x240 integer pixel perfection.
- Built `TitleScene` with golden Triforce rendering, retro typography, and blinking prompt.
- Verified: `make` (0 warnings), `make test` (4/4 passing).
