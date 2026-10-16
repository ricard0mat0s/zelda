# 03: Overworld Map & 4-Directional Player Movement

**What to build:** An explorable 40x30 tile overworld with 16x16 pixel retro graphics (forest glade, stone paths, perimeter trees, water pond, ruins), solid obstacle collisions, smooth 2D camera following Link within clamped boundaries, and 4-directional walking controls.

**Blocked by:** 01 (Project Scaffold, Raylib C++ & Retro Title Screen), 02 (Asset Pipeline & Audio)

**Status:** resolved

- [x] Hero moves smoothly in 4 directions using `W, A, S, D` or `Arrow Keys` with idle and walking animation frames.
- [x] Diagonal movement velocity is normalized by $\approx 0.7071$ so diagonal speed matches cardinal speed.
- [x] Perimeter trees, pond water, and stone walls obstruct movement via solid bounding box collisions and sliding resolution.
- [x] Smooth 2D camera tracks player and remains clamped strictly within the 40x30 map borders (640x480 world).
- [x] Location banner "HYLIA GLADE" displays on entry and fades out over 3.5 seconds.
- [x] Headless unit tests verify movement math, map solids, and collision sliding.

## Resolution Summary

- Created `src/core/Movement.hpp` and `src/core/Movement.cpp` implementing normalized diagonal velocity and persistent facing directions.
- Created `src/core/MapData.hpp` and `src/core/MapData.cpp` defining 40x30 world grid (640x480), perimeter tree barriers, water pond, ruins, paths, and solid sliding AABB collision resolution.
- Created `src/core/Player.hpp` and `src/core/Player.cpp` with feet-aligned hitbox (12x10), walk cycle frame switching (0.15s), and decoupled simulation update.
- Implemented `src/render/MapRenderer.cpp` drawing ground and obstacle layers with camera viewport culling.
- Implemented `src/app/OverworldScene.cpp` handling WASD/Arrow controls, smooth clamped camera follow, Elder Shrine pedestal rendering, and floating location banner.
- Verified: `make test` (9/9 passing headlessly), `make` (clean build), and live runtime verification with Raylib window.
