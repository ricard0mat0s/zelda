# Issue tracker: GitHub

Issues, specs, and pull requests for this repo live on GitHub (`ricard0mat0s/zelda`). Use the `gh` CLI for all operations.

## Issue conventions

- **Create an issue**: `gh issue create --title "..." --body "..."`. Use a heredoc for multi-line bodies.
- **Read an issue**: `gh issue view <number> --comments`, filtering comments by `jq` and also fetching labels.
- **List issues**: `gh issue list --state open --json number,title,body,labels,comments --jq '[.[] | {number, title, body, labels: [.labels[].name], comments: [.comments[].body]}]'` with appropriate `--label` and `--state` filters.
- **Claim an issue**: `gh issue edit <number> --add-assignee @me`
- **Comment on an issue**: `gh issue comment <number> --body "..."`
- **Apply / remove labels**: `gh issue edit <number> --add-label "..."` / `--remove-label "..."`
- **Close**: `gh issue close <number> --comment "..."` (or let a linked Pull Request close it via `Closes #<number>`).

Infer the repo from `git remote -v`; `gh` does this automatically when run inside a clone.

## Pull request & release workflow

Each implementation ticket is delivered on its own feature branch, submitted via a GitHub Pull Request, squash-merged at Checkpoint 2, and evaluated against the Release Gate:

- **Create a feature branch**: `git checkout -b feat/<issue-number>-<slug>` (or `fix/<issue-number>-<slug>`) from the base branch (`master`).
- **Verify & review locally first**: Run `make test` and `make` (plus `python3 tools/playtest.py` for visual/scene changes), commit your changes using Conventional Commits (`feat(scope): ... (#<issue-number>)`), and run `/code-review` against `master` before opening the PR.
- **Push and open a PR**:
  ```bash
  git push -u origin feat/<issue-number>-<slug>
  gh pr create --base master --title "feat(scope): ... (#<issue-number>)" --body "Closes #<issue-number>..."
  ```
- **Checkpoint 2 (Squash-Merge)**: After human approval of the PR summary and study references, squash-merge into `master`:
  ```bash
  gh pr merge <pr-number> --squash --delete-branch
  git checkout master && git pull --ff-only
  ```
- **Stage 4 Release Gate (`/release`)**:
  - When all child tickets of a Parent Spec close $\rightarrow$ trigger a `MINOR` release (`v0.X.0`, starting from `v0.1.0`).
  - When a standalone `fix:` ticket closes $\rightarrow$ trigger a `PATCH` release (`v0.X.Y`).
  - Full release runbook (Keep a Changelog + `.tar.gz` binary/assets bundle + `gh release create`) lives in `.agent/skills/release/SKILL.md`.
- **Read a PR**: `gh pr view <number> --comments` and `gh pr diff <number>` for the diff.
- **List open PRs**: `gh pr list --state open --json number,title,body,headRefName,baseRefName,labels,author,comments`
- **Comment / edit / close / merge a PR**: `gh pr comment`, `gh pr edit`, `gh pr close`, `gh pr merge`.

## Pull requests as a triage surface

**PRs as a request surface: no.** _(Set to `yes` if this repo treats external PRs as feature requests; `/triage` reads this flag.)_

When set to `yes`, PRs run through the same labels and states as issues, using the `gh pr` equivalents:

- **Read a PR**: `gh pr view <number> --comments` and `gh pr diff <number>` for the diff.
- **List external PRs for triage**: `gh pr list --state open --json number,title,body,labels,author,authorAssociation,comments` then keep only `authorAssociation` of `CONTRIBUTOR`, `FIRST_TIME_CONTRIBUTOR`, or `NONE` (drop `OWNER`/`MEMBER`/`COLLABORATOR`).
- **Comment / label / close**: `gh pr comment`, `gh pr edit --add-label`/`--remove-label`, `gh pr close`.

GitHub shares one number space across issues and PRs, so a bare `#42` may be either: resolve with `gh pr view 42` and fall back to `gh issue view 42`.

## When a skill says "publish to the issue tracker"

Create a GitHub issue via `gh issue create`.

## When a skill says "fetch the relevant ticket"

Run `gh issue view <number> --comments` (or `gh pr view <number> --comments` if referring to a PR).

## Wayfinding operations

Used by `/wayfinder`. The **map** is a single issue with **child** issues as tickets.

- **Map**: a single issue labelled `wayfinder:map`, holding the Notes / Decisions-so-far / Fog body. `gh issue create --label wayfinder:map`.
- **Child ticket**: an issue linked to the map as a GitHub sub-issue (`gh api` on the sub-issues endpoint). Where sub-issues aren't enabled, add the child to a task list in the map body and put `Part of #<map>` at the top of the child body. Labels: `wayfinder:<type>` (`research`/`prototype`/`grilling`/`task`). Once claimed, the ticket is assigned to the driving dev.
- **Blocking**: GitHub's **native issue dependencies**, the canonical, UI-visible representation. Add an edge with `gh api --method POST repos/<owner>/<repo>/issues/<child>/dependencies/blocked_by -F issue_id=<blocker-db-id>`, where `<blocker-db-id>` is the blocker's numeric **database id** (`gh api repos/<owner>/<repo>/issues/<n> --jq .id`, _not_ the `#number` or `node_id`). GitHub reports `issue_dependencies_summary.blocked_by` (open blockers only, the live gate). Where dependencies aren't available, fall back to a `Blocked by: #<n>, #<n>` line at the top of the child body. A ticket is unblocked when every blocker is closed.
- **Frontier query**: list the map's open children (`gh issue list --state open`, scoped to the map's sub-issues / task list), drop any with an open blocker (`issue_dependencies_summary.blocked_by > 0`, or an open issue in the `Blocked by` line) or an assignee; first in map order wins.
- **Claim**: `gh issue edit <n> --add-assignee @me`, the session's first write.
- **Resolve**: `gh issue comment <n> --body "<answer>"`, then `gh issue close <n>` (or open/merge the linked PR with `Closes #<n>`), then append a context pointer (gist + link) to the map's Decisions-so-far.
