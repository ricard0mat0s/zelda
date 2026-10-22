# 11: LocalStorage Save Persistence & Firebase Deployment

**What to build:** Auto-saving checkpoints (sword, bow, keys, dungeon progress) in LocalStorage with player respawn at room entrance, plus production build optimization and Firebase Hosting deployment configuration.

**Blocked by:** 10 (Boss Encounter — Ancient Eye Golem)

**Status:** resolved

- [x] Unit tests for `SaveSystem` verify data serialization, deserialization, and fallback for corrupted storage.
- [x] Checkpoints save automatically when obtaining items, clearing rooms, and unlocking doors.
- [x] Dying respawns player at current room entrance with retained inventory.
- [x] `make` generates an optimized native C++20 production binary in `bin/zelda_game`.
- [x] Dependency tracking (`-MMD -MP`) and hermetic static Raylib linkage configured for standalone execution.
