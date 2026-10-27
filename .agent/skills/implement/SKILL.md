---
name: implement
description: "Auto-pick or implement a ready-for-agent GitHub issue, verify with TDD and headless playtest, run parallel code review, open/merge a PR at Checkpoint 2, and evaluate the Release Gate (/release)."
disable-model-invocation: true
---

Implement the work described by the user or auto-picked from the GitHub issue frontier.

The issue tracker, PR, and release conventions are documented in `docs/agents/issue-tracker.md` and `AGENT.md`.

## End-to-End Implementation & Release Loop

1. **Frontier Auto-Pickup, Claim & Branch**:
   - If no specific issue number was passed, query the **Frontier**: list open issues labeled `ready-for-agent` (`gh issue list --state open --label ready-for-agent --json number,title,body,assignees`) and select the first unassigned issue whose `Blocked by` issues are all closed.
   - Fetch the full ticket (and parent spec if referenced) via `gh issue view <number> --comments`.
   - Claim the issue: `gh issue edit <number> --add-assignee @me`.
   - Create and switch to a dedicated branch from `master`: `git checkout master && git pull --ff-only && git checkout -b feat/<number>-<slug>` (or `fix/<number>-<slug>` for bugfixes).
   - Restate what will be built, identify the public seams, and include a brief **`📚 Why & Study References`** note on the seam choice.

2. **Drive TDD at Domain Seams (`/tdd`)**:
   - Use `/tdd` (Red-Green-Refactor) at pre-agreed seams in `src/core/`.

3. **Verify As You Go (`make test`, `make`, & `/playtest-agent`)**:
   - Compile and run the headless test suite (`make test`) and full binary (`make`) with `0 errors` and `0 warnings` (`-Wall -Wextra`).
   - When UI, rendering, scenes, or gameplay interactions are touched, run `/playtest-agent` (`python3 tools/playtest.py`) to verify JSON state and screenshots headlessly.

4. **Commit with Conventional Commits & Run `/code-review`**:
   - Commit changes using [Conventional Commits 1.0.0](https://www.conventionalcommits.org/):
     - `git commit -m "feat(<scope>): <description> (#<number>)"` or `fix(<scope>): <description> (#<number>)`
   - Run `/code-review` locally against `master` (Standards + Spec sub-agents in parallel).
   - Fix any critical or warning findings before pushing.

5. **Push, Open PR & Pause at Checkpoint 2**:
   - Push the branch and open a Pull Request:
     ```bash
     git push -u origin <branch-name>
     gh pr create --base master --title "<type>(<scope>): <summary> (#<number>)" --body "Closes #<number>..."
     ```
   - **🛑 CHECKPOINT 2**: Present the PR URL, verification summary (`make test`, `/playtest-agent`, `/code-review`), and a **`📚 Why & Study References`** block. Ask the user to approve squash-merging the PR.

6. **Stage 4: Squash-Merge & Evaluate Release Gate (`/release`)**:
   - Once the user approves Checkpoint 2, squash-merge the PR and sync `master`:
     ```bash
     gh pr merge <pr-number> --squash --delete-branch
     git checkout master && git pull --ff-only
     ```
   - **Evaluate the Release Trigger** (as defined in `.agent/skills/release/SKILL.md`):
     - **Case A (Milestone Complete)**: All child tickets of the ticket's `## Parent` Spec are now closed $\rightarrow$ Invoke `/release` to propose a `MINOR` release (`v0.X.0` at **Checkpoint 3**).
     - **Case B (Standalone Bugfix)**: The merged issue was a standalone `fix:` issue with no Parent Spec $\rightarrow$ Invoke `/release` to propose a `PATCH` release (`v0.X.Y` at **Checkpoint 3**).
     - **Case C (More Sibling Tickets Open)**: Explain why a release is deferred until the Parent Spec milestone is complete, and automatically continue to Step 1 for the next unblocked `ready-for-agent` ticket on the Frontier.

---

## Project Considerations & Guidelines (Game Dev & Architecture)

### 1. Seam Discipline: Decouple Domain Logic from Game Engine (Raylib)
* **Where TDD is mandatory / strongly recommended**:
  - Pure domain models, state machines, math, and persistence under `src/core/`.
  - Examples: `SaveSystem`, `Inventory`, `Health`, `Combat`, `Shield`, `Slime`, `Dungeon`, `Projectile`, `DungeonEnemies`, `BossGolem`, `SoundGenerator`.
  - These MUST be pure C++20 classes/functions with zero dependency on Raylib windowing/OpenGL context so they compile and run headlessly in `bin/test_runner` via `make test` in milliseconds.
* **Where `/playtest-agent` & Clean Build are used**:
  - Presentation, Raylib window/scene lifecycle (`TitleScene`, `OverworldScene`, `DungeonScene`), GPU texture uploads (`AssetManager`), audio device playback (`AudioPlayer`), and visual HUD layout (`UIRenderer`).
  - Verify these via clean compilation (`make`) and headless screenshot/state inspection (`python3 tools/playtest.py`).

### 2. Testing & Tooling Standards
* **Test Runner**: Headless C++20 test suite (`make test` -> `./bin/test_runner`).
* **Compiler & Strictness**: `g++ -std=c++20 -Wall -Wextra -O2` (`make`).
* **Build Check**: Standalone binary build (`make` -> `./bin/zelda_game`).

### 3. Review Order & Git Hygiene
* `code-review` requires a git diff to inspect. Always commit your changes on your feature branch first so that `git diff master...HEAD` provides full visibility to the Standards and Spec sub-agents before you push and open the PR.
* One ticket / vertical slice per branch and Pull Request. Do not combine multiple unrelated tickets into a single giant PR.
