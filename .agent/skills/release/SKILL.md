---
name: release
description: "Evaluate release triggers, calculate Semantic Version bumps (v0.1.0 baseline), update CHANGELOG.md, package the Linux binary + assets tarball, and publish a GitHub Release via gh release create."
---

# Release Skill (`/release`)

Cut a reproducible, versioned GitHub Release when a **Parent Spec milestone completes** (`MINOR` bump: `v0.X.0`) or a **standalone bugfix issue merges** (`PATCH` bump: `v0.X.Y`), starting from the initial `v0.1.0` baseline.

---

## 1. When to Trigger a Release (Release Gate Rules)

After squash-merging a Pull Request into `master` (`git checkout master && git pull --ff-only`):

1. **Initial Baseline (`v0.1.0`)**:
   - If `git tag -l` is empty and a milestone or initial playable baseline is ready to be tagged, propose **`v0.1.0`**.
2. **Parent Spec / Milestone Complete (`MINOR` -> `v0.X.0`)**:
   - Check if the merged ticket references a `## Parent` issue (e.g., `#10`).
   - Query open child issues referencing that parent:
     ```bash
     gh issue list --state open --search "in:body #<parent-number>" --json number,title
     ```
   - If **zero** child tickets remain open for that Parent Spec, close the Parent Spec issue (`gh issue close <parent-number> --comment "All child tickets merged. Releasing in v0.X.0."`) and trigger a **MINOR release (`v0.X.0`)**.
3. **Standalone Bugfix Merged (`PATCH` -> `v0.X.Y`)**:
   - If a standalone `fix:` issue (with no Parent Spec) was merged to `master`, trigger a **PATCH release (`v0.X.Y`)**.
4. **In-Progress Milestone (No Release Yet)**:
   - If sibling tickets under the same Parent Spec are still open, **do not cut a release yet**—explain why (preventing half-built feature releases) and automatically pick up the next unblocked `ready-for-agent` ticket on the Frontier.

---

## 2. Checkpoint 3: Propose Version & Changelog to the User

Before creating tags or publishing to GitHub, pause at **Checkpoint 3** and present:
1. **Proposed Version**: e.g., `v0.1.0` or `v0.2.0` (calculated from `git describe --tags --abbrev=0 2>/dev/null || echo "none"`).
2. **Drafted `CHANGELOG.md` Section**: Grouped under `### Added`, `### Changed`, `### Fixed` following [Keep a Changelog 1.1.0](https://keepachangelog.com/en/1.1.0/), derived from `git log <last-tag>..HEAD --oneline` (or full commit history for `v0.1.0`).
3. **`📚 Why & Study References`**: Explain why this release boundary was chosen and cite relevant release engineering literature (*Semantic Versioning 2.0.0*, *Keep a Changelog*, *Continuous Delivery*).

Wait for the user's confirmation before executing Section 3.

---

## 3. Execute the Release Pipeline

Once confirmed by the user, run these deterministic steps:

### Step 1: Clean Verification Build & Headless Tests
Verify the `master` branch compiles cleanly with zero warnings and passes all headless unit tests:
```bash
make clean && make test && make
```

### Step 2: Update `CHANGELOG.md`
Create or update `CHANGELOG.md` at the repository root following [Keep a Changelog 1.1.0](https://keepachangelog.com/en/1.1.0/):
- Include version header: `## [0.X.Y] - YYYY-MM-DD`
- List user-visible changes under `### Added`, `### Changed`, `### Fixed` with issue/PR references (`(#<number>)`).

### Step 3: Commit Release & Create Annotated Git Tag
```bash
git add CHANGELOG.md
git commit -m "chore(release): v0.X.Y"
git tag -a v0.X.Y -m "Release v0.X.Y"
git push origin master --follow-tags
```

### Step 4: Package Binary & Assets Tarball
Bundle the compiled `bin/zelda_game` executable and `assets/` directory into a self-contained Linux release archive inside `/tmp` (so build artifacts don't pollute git status):
```bash
VERSION="v0.X.Y"
STAGE_DIR="/tmp/zelda_game-${VERSION}-linux-x86_64"
TARBALL="/tmp/zelda_game-${VERSION}-linux-x86_64.tar.gz"

rm -rf "${STAGE_DIR}" "${TARBALL}"
mkdir -p "${STAGE_DIR}/bin"
cp bin/zelda_game "${STAGE_DIR}/bin/"
cp -r assets "${STAGE_DIR}/assets"
cp CHANGELOG.md "${STAGE_DIR}/"

tar -czf "${TARBALL}" -C /tmp "zelda_game-${VERSION}-linux-x86_64"
```

### Step 5: Publish GitHub Release via `gh release create`
Publish the release notes and attach the `.tar.gz` bundle to GitHub Releases:
```bash
gh release create "v0.X.Y" "${TARBALL}" \
  --title "v0.X.Y — <Milestone or Release Title>" \
  --notes-file <(awk '/^## \[0\.X\.Y\]/{flag=1; next} /^## \[/{flag=0} flag' CHANGELOG.md)
```
Clean up the temporary staging files:
```bash
rm -rf "${STAGE_DIR}" "${TARBALL}"
```

### Step 6: Report Release URL & Study Digest
Provide the published GitHub Release URL (`gh release view v0.X.Y --web` / URL output) and a concluding **`📚 Why & Study References`** summary.
