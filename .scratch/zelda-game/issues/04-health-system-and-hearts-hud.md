# 04: Pure Domain HealthSystem & Retro Hearts HUD

**What to build:** A decoupled, test-driven health model managing hearts and temporary invulnerability in native C++, connected to a persistent HUD overlay rendering 3 retro hearts with half-heart precision and flashing invulnerability.

**Blocked by:** 03 (Overworld Map & 4-Directional Player Movement)

**Status:** resolved

- [x] TDD unit tests for `Health` verify damage reduction, half-heart precision, healing, max health caps, and invulnerability timer countdowns.
- [x] `UIRenderer` renders the classic `- LIFE -` HUD header and 3 heart containers (`heart_full`, `heart_half`, `heart_empty`).
- [x] Taking damage deducts hearts on the HUD, plays `playerHurt` SFX, and triggers player sprite flashing invulnerability for 1.0 second.
- [x] Zero HP triggers death state, freezes movement, displays a red tint and Game Over overlay with `ENTER` retry prompt.
- [x] Debug controls `H` (take 1 damage) and `J` (heal 1 HP) allow rapid gameplay verification.

## Resolution Summary

- Created `src/core/Health.hpp` and `src/core/Health.cpp` managing 3 max hearts (6 HP units), damage resolution, healing, 1.0s invulnerability timer, death state, and `isFlickerVisible()` 0.08s blinking cycle.
- Integrated `Health` into `src/core/Player.hpp` and `src/core/Player.cpp` with dead movement freeze and damage forwarding.
- Built `src/render/UIRenderer.hpp` and `src/render/UIRenderer.cpp` rendering the retro `- LIFE -` header, dynamic heart states, and Game Over retry prompt.
- Integrated hearts HUD and player invulnerability flashing into `src/app/OverworldScene.cpp`, connecting `AudioPlayer::playPlayerHurt()` on damage.
- Verified: `make test` (11/11 passing headlessly), `make` (clean build), and live runtime verification with Raylib window.
