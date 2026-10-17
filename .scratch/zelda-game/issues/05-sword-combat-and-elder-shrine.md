# 05: Sword Combat & Elder Shrine Pedestal

**What to build:** An interactive stone pedestal in the overworld with a typewriter dialogue box ("It's dangerous to go alone! Take this.") that awards the Hero's Sword, enabling 4-directional melee slashing with sound effects and active hitboxes in native C++.

**Blocked by:** 02 (Asset Pipeline & Audio), 04 (HealthSystem & Hearts HUD)

**Status:** resolved

- [x] Player starts unarmed and cannot attack until obtaining the sword.
- [x] Approaching the shrine pedestal (within 26px proximity) displays an `[E] EXAMINE` prompt; pressing `ENTER` or `E` opens a retro typewriter dialogue box.
- [x] Dialogue reveals text letter by letter; pressing `E` fast-forwards or advances the dialogue.
- [x] Finishing the dialogue grants the sword, plays `secretChime` SFX, and updates the shrine sprite to `shrine_pedestal_empty`.
- [x] Pressing `SPACE` or `Z` performs a sword slash animation in the current facing direction (`slash_down`, `slash_up`, `slash_left`, `slash_right`) with `swordSlash` sound effect.
- [x] Directional attack hitbox is active only during the swing animation frames (0.25s) with a short cooldown, locking movement during the slash.
- [x] Headless unit tests verify inventory, typewriter mechanics, directional sword hitboxes, knockback, and attack lifecycle.

## Resolution Summary

- Created `src/core/Inventory.hpp` managing sword, bow, and keys state.
- Created `src/core/Typewriter.hpp` and `src/core/Typewriter.cpp` with progressive character disclosure (0.028s/char) and skip capabilities.
- Created `src/core/Combat.hpp` and `src/core/Combat.cpp` calculating directional sword hitboxes, knockback vectors, and attack validation.
- Integrated inventory and attack state machine into `src/core/Player.hpp` and `src/core/Player.cpp` with movement locking during slash.
- Created `src/render/DialogueBox.hpp` and `src/render/DialogueBox.cpp` rendering a retro bordered text container with blinking continue prompt.
- Integrated shrine proximity examination, dialogue callbacks, secret fanfare, and sword slash rendering into `src/app/OverworldScene.cpp`.
- Verified: `make test` (15/15 passing headlessly), `make` (clean build), and live runtime verification with Raylib window.
