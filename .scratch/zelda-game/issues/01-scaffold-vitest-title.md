# 01: Project Scaffold, Vitest & Retro Title Screen

**What to build:** A working web app skeleton running Phaser 3 with TypeScript, Vite, and Vitest. When launched, the browser renders a pixel-perfect arcade title screen with a retro logo and "Press SPACE to Start" prompt that transitions to the game scene.

**Blocked by:** None (can start immediately)

**Status:** resolved

- [x] `npm run dev` boots the local dev server without errors.
- [x] `npx vitest run` runs successfully and reports 0 failing tests.
- [x] `npx tsc --noEmit` passes with strict typechecking and 0 errors.
- [x] Retro pixel title screen renders centered with integer pixel scaling.
- [x] Pressing `SPACE` triggers transition from TitleScene to OverworldScene.

## Resolution Summary

- Initialized Vite + Phaser 3 + TypeScript + Vitest stack in strict mode.
- Created `src/config.ts` defining retro 320x240 resolution, integer pixel scaling, and Arcade physics with zero gravity.
- Wrote TDD tests in `src/config.test.ts` verifying configuration parameters and decoupling from headless browser constraints.
- Built `BootScene`, `TitleScene` (with Triforce graphics, floating title typography, blinking prompt, controls hint), and `OverworldScene` transition target.
- Verified: `npx vitest run` (2/2 passing), `npx tsc --noEmit` (clean), and `npm run build` (bundled static output in `dist/`).
