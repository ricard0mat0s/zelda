# 04: Pure Domain HealthSystem & Retro Hearts HUD

**What to build:** A decoupled, test-driven health model managing hearts and temporary invulnerability, connected to a persistent HUD overlay scene that displays 3 retro hearts with half-heart precision.

**Blocked by:** 03 (Overworld Map & 4-Directional Player Movement)

**Status:** resolved

- [x] TDD unit tests for `HealthSystem` verify damage reduction, healing, max health caps, and invulnerability timer countdowns.
- [x] `UIScene` overlay renders on top of the active game scene showing 3 full hearts (6 HP units).
- [x] Taking damage deducts hearts on the HUD and triggers player sprite flashing invulnerability for 1.0 second.
- [x] Zero HP triggers death state notification.

## Resolution Summary

- Created `src/systems/HealthSystem.ts` managing 3 max hearts (6 HP units), half-heart precision (`getHeartUnits()`), healing, invulnerability countdown, and death state. Verified with 6 unit tests in `src/systems/HealthSystem.test.ts`.
- Created 16x16 pixel art textures for `heart_full`, `heart_half`, and `heart_empty` in `src/utils/AssetGenerator.ts`. Verified in `src/utils/AssetGenerator.test.ts`.
- Implemented `UIScene` overlay in `src/scenes/UIScene.ts` rendering the retro `- LIFE -` HUD header and dynamic hearts that re-render upon health state changes.
- Integrated `HealthSystem` into `Player.ts`:
  - `takeDamage()` plays `playerHurt` SFX and triggers camera hit flash.
  - Flashing alpha effect while invulnerable for 1.0 second.
  - Death handling with movement disable and red tint.
  - Added debug test controls (`H` to damage 1 HP, `J` to heal 1 HP).
- Launched `UIScene` from `OverworldScene.ts` and registered it in `src/main.ts`.
- Verified: `npx vitest run` (25/25 passing across 8 suites), `npx tsc --noEmit` (clean), and `npm run build` (successful).
