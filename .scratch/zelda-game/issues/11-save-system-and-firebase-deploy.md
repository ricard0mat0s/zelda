# 11: LocalStorage Save Persistence & Firebase Deployment

**What to build:** Auto-saving checkpoints (sword, bow, keys, dungeon progress) in LocalStorage with player respawn at room entrance, plus production build optimization and Firebase Hosting deployment configuration.

**Blocked by:** 10 (Boss Encounter — Ancient Eye Golem)

**Status:** ready-for-agent

- [ ] Unit tests for `SaveSystem` verify data serialization, deserialization, and fallback for corrupted storage.
- [ ] Checkpoints save automatically when obtaining items, clearing rooms, and unlocking doors.
- [ ] Dying respawns player at current room entrance with retained inventory.
- [ ] `npm run build` generates an optimized production bundle in `dist/`.
- [ ] `firebase.json` and `.firebaserc` are configured and ready for `firebase deploy --only hosting`.
