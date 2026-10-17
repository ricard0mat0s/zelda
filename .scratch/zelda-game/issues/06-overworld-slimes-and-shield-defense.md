# 06: Overworld Bouncing Slimes & Passive Shield Defense

**What to build:** Overworld green slime enemies that hop toward the player and damage on touch, paired with a passive frontal shield defense that repels head-on attacks.

**Blocked by:** 05 (Sword Combat & Elder Shrine Pedestal)

**Status:** resolved

- [x] Unit tests for shield defense verify projectile/damage deflection angle (facing direction vs collision normal).
- [x] Slimes bounce periodically and chase player when within detection radius.
- [x] Striking slimes with the sword pushes them back with knockback impulse, flashes white/red, and deducts HP.
- [x] Slimes die after 2 sword strikes with particle burst and defeat sound, with a 30% chance to drop a recovery heart.

## Resolution Summary
- Implemented `Shield` pure logic in `src/core/Shield.cpp` providing a 120-degree frontal defense cone. Verified through unit tests.
- Implemented `Slime` enemy state machine in `src/core/Slime.cpp` with hopping/idling cycles, player detection radius, knockback decay, flashing hurt effect, and 2-HP durability.
- Added recovery heart drop system in `src/core/ItemDrop.hpp`.
- Procedurally generated 16x16 pixel art sprites `slime_0` and `slime_1` in `src/render/GeneratedAssets.cpp`.
- Integrated active Slime instances, sword hit detection, frontal shield deflection with `hit` sound, heart drops, and collection into `src/app/OverworldScene.cpp`.
- Headless test suite expanded to 18 unit tests, fully passing.
