---
name: playtest-agent
description: Visually and interactively playtest The Legend of Arcade Hero using the screenshot and JSON state harness. Use when testing gameplay, verifying visual rendering, hunting for softlocks, or running automated QA sessions.
---

# Visual Playtest Agent Skill

Use `tools/playtest.py` (or `./bin/zelda_game --harness`) to control the game, inspect 960x720 screenshots with `read_file`, and read structured JSON game state.

## Interactive Step-by-Step Workflow

1. **Initialize a session**:
   ```bash
   # Start at the Title Screen:
   python3 tools/playtest.py init

   # Or start directly in a New Game on the Overworld:
   python3 tools/playtest.py init --new-game
   ```
2. **Inspect the visual output and state**:
   - The command prints the current `state.json` to stdout and writes the rendered frame to `.scratch/playtest/latest.png` (as well as `.scratch/playtest/step_NNN.png`).
   - Always call `read_file` on `.scratch/playtest/latest.png` (or `step_NNN.png`) to visually inspect the HUD, sprites, dialogue boxes, tile alignment, and animations.
3. **Send the next commands**:
   ```bash
   python3 tools/playtest.py step "move_to 200 196; face up; interact"
   ```
   Each `step` appends to the session history, fast-forwards deterministically, updates `.scratch/playtest/latest.png` and `state.json`, and saves a numbered `.scratch/playtest/step_NNN.png`.

## Harness Command Reference

Commands are separated by `;` or newlines:

- **Title / Menu / Dialogue**:
  - `start [N]` / `confirm [N]`: Presses `SPACE`/`ENTER` (starts game, advances dialogue, retries on death).
  - `new_game [N]`: Starts a fresh game from the Title screen (clears existing save).
  - `interact [N]`: Presses `E` and waits `N` frames (default 45 so typewriter text completes).
  - `dismiss_dialogue`: Advances and closes any open dialogue box.
  - `back [N]`: Presses `BACKSPACE` to return to the Title screen.
- **Movement**:
  - `up [N]`, `down [N]`, `left [N]`, `right [N]`: Holds direction for `N` frames (at 60 FPS).
  - `up_left [N]`, `up_right [N]`, `down_left [N]`, `down_right [N]`: Diagonal movement for `N` frames.
  - `face <up|down|left|right>`: Sets player facing direction.
  - `move_to <x> <y> [max_frames]`: Automatically walks toward `(x, y)` until within 2.5px or `max_frames` (default 300) elapses.
  - `wait [N]`: Advances `N` frames with no input.
- **Combat & Items**:
  - `attack [N]`: Swings sword (`SPACE`/`Z`) and waits `N` frames (default 16 for full swing).
  - `bow [N]` / `shoot [N]`: Fires an arrow (`X`) and waits `N` frames (default 15).
- **Debug / QA Shortcuts**:
  - `teleport <x> <y>`: Teleports player to `(x, y)` in the current scene.
  - `give_sword`, `give_bow`, `give_key`: Grants inventory items directly for targeted testing.
  - `enter_dungeon`: Enters Dungeon Room 1 from Overworld.
  - `room <1..6>`: Jumps to a specific Dungeon room (`1`: Entrance Foyer, `2`: Crystal Chasm, `3`: Treasure Chamber, `4`: Monster Arena, `5`: Boss Gate Chamber, `6`: Boss Sanctum).
  - `hurt [N]`, `heal [N]`: Triggers debug damage (`H`) or heal (`J`).
  - `screenshot <path.png>`, `state <path.json>`: Captures intermediate screenshots or state files mid-script.

## Key World Coordinates

- **Overworld**:
  - Spawn: `(200, 280)`
  - Elder Shrine Pedestal (Sword): `(200, 180)` — stand around `(192..200, 192..198)` facing `up` and `interact`.
  - Northern Ruins Dungeon Entrance: `(320, 32)` — approach `(320, 48)` and walk `up` into `(320, 32)` to enter the dungeon.
- **Dungeon Rooms**:
  - **Room 1 (Entrance Foyer)**: Pressure plate at `(160, 112)` opens North door `(152, 16)`. Stairs to exit at `(160, 208)`.
  - **Room 2 (Crystal Chasm)**: West door `(16, 112)` leads to Room 3. Crystal switch across chasm at `(160, 48)` — shoot arrow (`bow`) from `(152, 120)` facing `up` to open North door to Room 4.
  - **Room 3 (Treasure Chamber)**: Chest with Bow at `(160, 96)` — approach `(152, 108)` facing `up` and `interact`. East door `(288, 112)` returns to Room 2.
  - **Room 4 (Monster Arena)**: Defeat 2 Bats and 2 Skeletons to spawn Small Key at `(152, 112)`. East door leads to Room 5.
  - **Room 5 (Boss Gate Chamber)**: Locked North door at `(152, 24)` — approach facing `up` with Small Key to unlock and enter Room 6.
  - **Room 6 (Boss Sanctum)**: Ancient Eye Golem (`BossGolem`). Wait for `EyeCharging` phase, shoot eye with `bow` to stun (`Stunned`), then strike with `attack`. Collect `HeartContainer` at `(152, 104)` for Victory.
