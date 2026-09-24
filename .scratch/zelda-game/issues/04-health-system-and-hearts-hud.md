# 04: Pure Domain HealthSystem & Retro Hearts HUD

**What to build:** A decoupled, test-driven health model managing hearts and temporary invulnerability, connected to a persistent HUD overlay scene that displays 3 retro hearts with half-heart precision.

**Blocked by:** 03 (Overworld Map & 4-Directional Player Movement)

**Status:** ready-for-agent

- [ ] TDD unit tests for `HealthSystem` verify damage reduction, healing, max health caps, and invulnerability timer countdowns.
- [ ] `UIScene` overlay renders on top of the active game scene showing 3 full hearts (6 HP units).
- [ ] Taking damage deducts hearts on the HUD and triggers player sprite flashing invulnerability for 1.0 second.
- [ ] Zero HP triggers death state notification.
