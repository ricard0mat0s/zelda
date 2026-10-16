# 02: Asset Pipeline & Procedural 8-Bit Audio System

**What to build:** Zero-dependency procedural 16x16 pixel art asset pipeline and procedural 8-bit sound generator in C++ that synthesizes retro sound effects (sword slash, arrow shoot, enemy hit, secret chime, victory fanfare) and loads/exports textures without external asset bloat.

**Blocked by:** 01 (Project Scaffold, Raylib C++ & Retro Title Screen)

**Status:** resolved

- [x] Converted all 22 procedural pixel art definitions into C++ static tables in `src/render/GeneratedAssets.hpp`.
- [x] Built `AssetManager` in `src/render/AssetManager.cpp` loading 22 textures into GPU memory with point filtering and exporting `.png` files to `assets/sprites/`.
- [x] Pure C++ procedural audio generator in `src/core/SoundGenerator.cpp` synthesizing float PCM waveforms with ADSR envelopes, pitch jumps, slides, bitcrush, and delay.
- [x] Built `AudioPlayer` in `src/render/AudioPlayer.cpp` converting PCM to Raylib `Sound` and caching 9 retro sound presets.
- [x] Unit tests in `tests/test_runner.cpp` verify audio bounds `[-1.0, 1.0]`, signal presence, and 22 sprite dimensions headlessly.
- [x] Connected `AudioPlayer::playGameStart()` to `TitleScene` on `SPACE` or `ENTER`.

## Resolution Summary

- Created `tools/convert_assets.py` to parse legacy 16x16 pixel art grids into `src/render/GeneratedAssets.hpp` and `GeneratedAssets.cpp`.
- Implemented `AssetManager` with texture cache and disk exporter (`assets/sprites/*.png`).
- Ported ZzFX waveform synthesizer to pure C++ in `src/core/SoundGenerator.cpp` with 9 Zelda presets (`GameStart`, `SwordSlash`, `ArrowShoot`, `Hit`, `EnemyDefeat`, `SecretChime`, `ChestFanfare`, `PlayerHurt`, `BossRoar`).
- Implemented `AudioPlayer` wrapping Raylib `Wave`/`Sound` with lazy singleton caching.
- Verified: `make test` (6/6 passing headlessly), `make` (clean build), and live verification with Raylib window displaying title and exporting PNG assets.
