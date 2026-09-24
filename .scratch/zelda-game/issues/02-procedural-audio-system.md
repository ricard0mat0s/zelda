# 02: Procedural Web Audio Engine (ZzFX)

**What to build:** Zero-dependency procedural sound generator using Web Audio API that synthesizes 8-bit retro sound effects (sword slash, arrow shot, enemy hit, secret chime, victory fanfare) instantly without external audio files.

**Blocked by:** 01 (Project Scaffold, Vitest & Retro Title Screen)

**Status:** ready-for-agent

- [ ] Unit tests verify audio generator math and parameter bounds without browser crash.
- [ ] Pressing `SPACE` on the title screen plays a crisp retro start chime.
- [ ] AudioSystem exposes methods for all core SFX: `playSwordSlash()`, `playArrowShoot()`, `playHit()`, `playEnemyDefeat()`, `playSecretChime()`, `playChestFanfare()`, `playPlayerHurt()`.
- [ ] Audio context initializes cleanly on first user interaction without browser warnings.
