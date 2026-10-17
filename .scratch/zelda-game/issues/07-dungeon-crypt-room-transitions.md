# 07: The Sunken Crypt Dungeon & Room Transitions

**What to build:** A dark stone dungeon entrance in the overworld leading into "The Sunken Crypt" (a multi-room indoor dungeon with room-by-room camera panning transitions).

**Blocked by:** 06 (Overworld Bouncing Slimes & Passive Shield Defense)

**Status:** resolved

- [x] Walking into the overworld dungeon stairway transitions player to Dungeon Room 1.
- [x] Stepping into room threshold doorways smoothly pans the camera to the adjacent room without reloading the scene.
- [x] Floor pressure plate in Room 1 triggers a door-open sound and unlocks passage forward.

## Resolution Summary
- Implemented pure domain model `Dungeon` in `src/core/Dungeon.cpp` and `src/core/Dungeon.hpp` representing the 6 dungeon chambers (Entrance Foyer, Crystal Chasm, Treasure Chamber, Monster Arena, Boss Gate, Boss Sanctum), door states, and sliding collision boundaries.
- Designed 7 procedural 16x16 pixel sprites (`dungeon_floor`, `dungeon_wall`, `dungeon_door_closed`, `dungeon_door_open`, `pressure_plate_up`, `pressure_plate_down`, `dungeon_stairs`) generated into static tables via `tools/convert_assets.py`.
- Implemented floor pressure plate mechanics in Room 1: stepping on the plate depresses it, plays `secretChime` sound effect, and permanently unlocks the northern portcullis into Room 2.
- Implemented smooth camera panning room transitions: walking across open doorway thresholds locks player input and smoothly pans the 2D camera viewport to the destination room without scene reloading.
- Connected Overworld Northern Stone Ruins stairs (col 20, row 2) to Dungeon Room 1 entrance and Room 1 south stairs back to the Overworld, preserving player health and equipment across scenes.
- Expanded headless test suite to 20 tests in `tests/test_runner.cpp`, verifying room bounds, pressure plate triggers, and camera pan interpolation.
