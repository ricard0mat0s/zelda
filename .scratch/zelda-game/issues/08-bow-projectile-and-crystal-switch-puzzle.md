# 08: Bow & Quiver Weapon with Crystal Switch Puzzle

**What to build:** A golden treasure chest in Dungeon Room 3 granting the Bow & Quiver, ranged arrow firing mechanics, and a crystal switch puzzle across a deep pit that opens an iron gate when struck.

**Blocked by:** 07 (The Sunken Crypt Dungeon & Room Transitions)

**Status:** resolved

- [x] Interacting with the golden chest plays fanfare SFX and unlocks Bow in HUD inventory.
- [x] Pressing `X` shoots an arrow projectile in the player's facing direction.
- [x] Arrows fly at high velocity and break on colliding with walls or obstacles.
- [x] Shooting the crystal switch across the chasm plays the secret puzzle chime and lowers the locked gate.

## Resolution Summary
- Implemented `Arrow` projectile system in `src/core/Projectile.cpp` and `src/core/Projectile.hpp` with 4-directional velocity (240 px/s), lifetime timeout, and precise directional AABB hitboxes.
- Implemented golden treasure chest interaction in Dungeon Room 3: interacting with `[E] OPEN` grants the Bow & Quiver to inventory, plays `chestFanfare` audio, displays dialogue instruction, and unlocks the secondary item slot in the HUD.
- Implemented ranged secondary attack (`X` key) allowing Link to fire arrows both inside the dungeon and in the overworld to defeat slimes at range.
- Implemented the Crystal Switch puzzle across the chasm in Dungeon Room 2: arrows fly over chasm pits; striking the crystal switch activates it, plays `secretChime` SFX, and unlocks the north portcullis into Room 4 (Monster Arena).
- Generated 10 new 16x16 pixel art sprites (`chest_closed`, `chest_open`, `bow_icon`, `arrow_up`, `arrow_down`, `arrow_left`, `arrow_right`, `crystal_switch_blue`, `crystal_switch_active`, `chasm`) bringing total assets to 41, and exported all PNGs to `assets/sprites/`.
- Expanded headless test suite to 23 tests in `tests/test_runner.cpp`, verifying projectile flight, wall collision, chest opening logic, and crystal switch triggers.
