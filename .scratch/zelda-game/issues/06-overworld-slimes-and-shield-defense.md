# 06: Overworld Bouncing Slimes & Passive Shield Defense

**What to build:** Overworld green slime enemies that hop toward the player and damage on touch, paired with a passive frontal shield defense that repels head-on attacks.

**Blocked by:** 05 (Sword Combat & Elder Shrine Pedestal)

**Status:** ready-for-agent

- [ ] Unit tests for shield defense verify projectile/damage deflection angle (facing direction vs collision normal).
- [ ] Slimes bounce periodically and chase player when within detection radius.
- [ ] Striking slimes with the sword pushes them back with knockback impulse, flashes white/red, and deducts HP.
- [ ] Slimes die after 2 sword strikes with particle burst and defeat sound, with a 30% chance to drop a recovery heart.
