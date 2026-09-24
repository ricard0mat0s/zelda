---
name: implement
description: "Implement a piece of work based on a spec or set of tickets."
disable-model-invocation: true
---

Implement the work described by the user in the spec or tickets.

## Workflow

1. **Read & Clarify**: Read the ticket or spec. Restate what will be built and identify the public seams.
2. **Drive TDD at Domain Seams**: Use `/tdd` where possible at pre-agreed seams.
3. **Verify As You Go**:
   - Run typechecking regularly (`npx tsc --noEmit`).
   - Run single test files as you iterate (`npx vitest run <file>`).
4. **Full Verification**: Run the full test suite once at the end (`npx vitest run`), and ensure production builds succeed (`npm run build`).
5. **Commit & Review**:
   - Commit your work to the current branch (`git commit -m "feat: ..."`).
   - Use `/code-review` against the fixed point / previous commit (`HEAD~1` or base branch).
   - Address any critical findings.

---

## Project Considerations & Guidelines (Game Dev & Architecture)

### 1. Seam Discipline: Decouple Domain Logic from Game Engine (Phaser 3)
* **Where TDD is mandatory / strongly recommended**:
  - Pure domain models, state machines, math, and persistence.
  - Examples: `SaveSystem`, `InventorySystem`, `HealthSystem`, `CombatCalculations` (shield blocking angle, damage, knockback vectors), `PuzzleStateMachine`, `BossGolemStateMachine`.
  - These MUST be pure TypeScript classes/functions with zero dependency on `Phaser.Scene` or WebGL rendering so they run in Vitest within milliseconds.
* **Where TDD should NOT be forced**:
  - Presentation, Phaser scene lifecycle (`preload`, `create`, `update`), sprite animations, tilemap rendering, audio playback (ZzFX triggers), and Arcade Physics body tuning.
  - Trying to mock Phaser WebGL/Canvas creates fragile tests that hinder gameplay feel iteration.
  - Verify these via manual playtesting and visual inspection.

### 2. Testing & Tooling Standards
* **Test Runner**: Vitest (`npx vitest run`).
* **Type Safety**: TypeScript strict mode (`npx tsc --noEmit`).
* **Bundle Check**: Vite build (`npm run build`).

### 3. Review Order & Git Hygiene
* In standard `implement`, `code-review` requires a git diff to inspect. Always commit your changes first (or ensure interim commits exist) so that `git diff <fixed-point>...HEAD` provides full visibility to the Standards and Spec sub-agents.
* One ticket / vertical slice per implementation cycle. Do not combine multiple tickets into a single giant PR or commit.

