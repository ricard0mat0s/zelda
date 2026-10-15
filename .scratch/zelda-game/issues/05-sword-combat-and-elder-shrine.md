# 05: Sword Combat & Elder Shrine Pedestal

**What to build:** An interactive stone pedestal in the overworld with a typewriter dialogue box ("It's dangerous to go alone! Take this.") that awards the Hero's Sword, enabling 4-directional melee slashing with sound effects and active hitboxes.

**Blocked by:** 02 (Procedural Web Audio Engine), 04 (Pure Domain HealthSystem & Retro Hearts HUD)

**Status:** resolved

- [x] Player starts unarmed and cannot attack until obtaining the sword.
- [x] Approaching the shrine pedestal and pressing `ENTER` or `E` opens a retro dialogue box with typewriter text.
- [x] Accepting grants the sword and updates inventory state.
- [x] Pressing `SPACE` or `Z` performs a sword slash animation in the current facing direction with slash sound effect.
- [x] Attack hitbox is active only during the swing animation frames with a short cooldown.
