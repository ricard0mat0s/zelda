# 02: Procedural Web Audio Engine (ZzFX)

**What to build:** Zero-dependency procedural sound generator using Web Audio API that synthesizes 8-bit retro sound effects (sword slash, arrow shot, enemy hit, secret chime, victory fanfare) instantly without external audio files.

**Blocked by:** 01 (Project Scaffold, Vitest & Retro Title Screen)

**Status:** resolved

- [x] Unit tests verify audio generator math and parameter bounds without browser crash.
- [x] Pressing `SPACE` on the title screen plays a crisp retro start chime.
- [x] AudioSystem exposes methods for all core SFX: `playSwordSlash()`, `playArrowShoot()`, `playHit()`, `playEnemyDefeat()`, `playSecretChime()`, `playChestFanfare()`, `playPlayerHurt()`.
- [x] Audio context initializes cleanly on first user interaction without browser warnings.

## Resolution Summary

- Created `src/systems/audio/zzfx.ts` implementing a zero-dependency procedural 8-bit sound generator with curated Zelda retro presets (`gameStart`, `swordSlash`, `arrowShoot`, `hit`, `enemyDefeat`, `secretChime`, `chestFanfare`, `playerHurt`, `bossRoar`).
- Wrote TDD tests in `src/systems/audio/zzfx.test.ts` verifying amplitude bounds, duration math, and preset validation.
- Implemented `src/systems/AudioSystem.ts` with singleton lifecycle, lazy user-gesture AudioContext activation, volume control, mute toggles, and memory buffer caching.
- Wrote unit tests in `src/systems/AudioSystem.test.ts` verifying headless resilience, volume clamping, and mute behavior.
- Hooked `AudioSystem.getInstance().playGameStart()` to `TitleScene` on `SPACE` key press before transitioning to `OverworldScene`.
- Verified: `npx vitest run` (11/11 passing), `npx tsc --noEmit` (clean), and `npm run build` (successful).
