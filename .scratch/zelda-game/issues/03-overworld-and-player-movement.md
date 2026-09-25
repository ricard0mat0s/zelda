# 03: Overworld Map & 4-Directional Player Movement

**What to build:** An explorable 40x30 tile overworld with procedural 16x16 pixel retro graphics (forest glade, stone paths, trees, water), solid obstacle collisions, camera following the hero within map boundaries, and 4-directional walking controls.

**Blocked by:** 01 (Project Scaffold, Vitest & Retro Title Screen)

**Status:** resolved

- [x] Hero moves smoothly in 4 directions using `W, A, S, D` or `Arrow Keys` with idle and walking animation frames.
- [x] Perimeter trees and water obstruct movement via solid arcade physics collisions.
- [x] Smooth camera tracks player and remains clamped strictly within the 40x30 map borders.
- [x] Integer pixel scaling preserves sharp 16x16 pixel art without blurring.

## Resolution Summary

- Created `src/entities/movement.ts` with pure math `computeMovementVelocity` handling cardinal directions, diagonal normalization, and facing persistence. Verified with 4 unit tests in `src/entities/movement.test.ts`.
- Created `src/utils/AssetGenerator.ts` generating authentic procedural 16x16 pixel art textures (`grass`, `path`, `tree`, `water`, `stone_wall`, and 8 Hero walk/idle frames across 4 directions). Verified with 3 unit tests in `src/utils/AssetGenerator.test.ts`.
- Created `src/constants/map.ts` defining 40x30 tile world dimensions (640x480) and 16px tile scale. Verified with unit tests in `src/constants/map.test.ts`.
- Implemented `Player` arcade physics entity in `src/entities/Player.ts` with WASD/Arrow input, walk animations, and feet-clamped depth-sorted hitbox.
- Built `OverworldScene` in `src/scenes/OverworldScene.ts` featuring full 40x30 grass base, winding stone paths, obstacle collisions (perimeter trees, water pond, ruins), smooth camera tracking, and location banner.
- Verified: `npx vitest run` (19/19 passing across 7 suites), `npx tsc --noEmit` (clean), and `npm run build` (successful).
