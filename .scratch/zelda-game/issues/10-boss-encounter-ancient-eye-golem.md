# 10: Boss Encounter — Ancient Eye Golem

**What to build:** The climax boss battle in Room 6: a 3-phase Ancient Eye Golem with a dynamic health bar, stone stomp projectiles, an eye-charging vulnerability window for arrow stun, and stunned sword attack openings.

**Blocked by:** 09 (Monster Arena & Small Key Progression)

**Status:** ready-for-agent

- [ ] Unit tests verify `BossGolemStateMachine` phase logic and stun timers.
- [ ] Phase 1: Golem stomps toward player and hurls rocks; body is armored and sword strikes bounce off.
- [ ] Phase 2: Golem pauses and eye turns glowing red to charge laser; hitting eye with an arrow stuns it for 3.5 seconds.
- [ ] Phase 3: Stunned golem takes full sword combo damage; after stun or damage cap, roars and speeds up.
- [ ] Defeat triggers screen shake, explosion particles, drops a Heart Container (+1 max heart), and opens the Victory screen.
