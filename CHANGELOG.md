# Changelog

All notable changes to **The Legend of Arcade Hero** will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2026-10-27

### Added
- **Project Scaffold & Retro Title Screen**: Initialized C++20 + Raylib engine with headless unit test runner (`bin/test_runner`) and animated retro title screen (#3).
- **Procedural 16x16 Pixel Art & 8-Bit Audio**: Added runtime procedural sprite generation (`GeneratedAssets`) and 8-bit WAV synthesis (`SoundGenerator`) (#4).
- **Overworld Map & Hero Movement**: Implemented 40x30 tilemap, 4-directional movement, corner-sliding collision physics, and smooth camera tracking (#5).
- **Health System & Retro Hearts HUD**: Added pure domain `Health` model, heart container HUD, and damage invulnerability frames (#6).
- **Sword Combat & Elder Shrine**: Added Elder Shrine sword pedestal, typewriter dialogue box, and 4-directional melee sword arc (#7).
- **Overworld Slimes & Passive Shield Defense**: Added bouncing overworld slimes, frontal shield projectile/contact blocking (`Shield`), and heart drops (#8).
- **The Sunken Crypt Dungeon**: Added multi-room dungeon layout (`Dungeon`) with smooth room-panning camera transitions (#9).
- **Bow & Quiver + Crystal Switch Puzzle**: Added arrow projectiles (`Projectile`) and crystal switch barrier puzzles (#10).
- **Monster Arena & Small Key Progression**: Added Bat and Skeleton enemies (`DungeonEnemies`), arena ambush locks, and small key doors (#11).
- **Boss Encounter — Ancient Eye Golem**: Implemented 3-phase `BossGolem` encounter, radial projectile patterns, Triforce Relic reward, and Victory screen (#12).
- **JSON Save System & Checkpoints**: Implemented `SaveSystem` (`savegame.json`), room-entrance respawn, and automatic checkpoint saving (#13).
- **Agentic Engineering Workflow & Headless Playtest Harness**: Added `tools/playtest.py` state/screenshot harness, `AGENT.md` mentor guidelines, and `/release` skill.

### Fixed
- **Build Dependency Tracking**: Added `-MMD -MP` header dependency flags in `Makefile` to prevent stale object ABI mismatches during incremental builds.
