#!/usr/bin/env python3
"""
Interactive & Batch Visual Playtest Controller for The Legend of Arcade Hero.

Allows an AI agent (or human developer) to step through the game interactively,
inspect 3x-upscaled (960x720) PNG screenshots, and read structured JSON state snapshots.
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
BINARY_PATH = REPO_ROOT / "bin" / "zelda_game"
DEFAULT_SESSION_DIR = REPO_ROOT / ".scratch" / "playtest"


def run_binary(script_path: Path, screenshot_path: Path, state_path: Path, show_window: bool = False) -> dict:
    if not BINARY_PATH.exists():
        raise RuntimeError(f"Game binary not found at {BINARY_PATH}. Run 'make all' first.")

    cmd = [
        str(BINARY_PATH),
        "--harness-file", str(script_path),
        "--screenshot", str(screenshot_path),
        "--state", str(state_path),
    ]
    if show_window:
        cmd.append("--show-window")

    res = subprocess.run(cmd, cwd=str(REPO_ROOT), capture_output=True, text=True)
    if res.returncode != 0:
        raise RuntimeError(f"zelda_game exited with code {res.returncode}:\n{res.stderr}\n{res.stdout}")

    if state_path.exists():
        return json.loads(state_path.read_text(encoding="utf-8"))
    return {}


def print_report(state: dict, screenshot_path: Path, step_screenshot_path: Path | None = None) -> None:
    print(f"Screenshot: {screenshot_path}")
    if step_screenshot_path and step_screenshot_path != screenshot_path:
        print(f"Step Image: {step_screenshot_path}")
    print("State JSON:")
    print(json.dumps(state, indent=2))


def cmd_init(args: argparse.Namespace) -> int:
    session_dir = Path(args.session_dir).resolve()
    session_dir.mkdir(parents=True, exist_ok=True)

    # Clear old step images
    for old_png in session_dir.glob("step_*.png"):
        old_png.unlink()

    history_file = session_dir / "history.txt"
    meta_file = session_dir / "meta.json"
    latest_png = session_dir / "latest.png"
    step_png = session_dir / "step_000.png"
    state_json = session_dir / "state.json"

    initial_cmd = "new_game" if args.new_game else "wait 2"
    history_file.write_text(initial_cmd + "\n", encoding="utf-8")
    meta_file.write_text(json.dumps({"step": 0}, indent=2), encoding="utf-8")

    state = run_binary(history_file, latest_png, state_json, args.show_window)
    if latest_png.exists():
        shutil.copyfile(latest_png, step_png)

    print_report(state, latest_png, step_png)
    return 0


def cmd_step(args: argparse.Namespace) -> int:
    session_dir = Path(args.session_dir).resolve()
    session_dir.mkdir(parents=True, exist_ok=True)

    history_file = session_dir / "history.txt"
    meta_file = session_dir / "meta.json"
    latest_png = session_dir / "latest.png"
    state_json = session_dir / "state.json"

    if not history_file.exists():
        history_file.write_text("wait 2\n", encoding="utf-8")

    step_num = 1
    if meta_file.exists():
        try:
            meta = json.loads(meta_file.read_text(encoding="utf-8"))
            step_num = int(meta.get("step", 0)) + 1
        except Exception:
            step_num = 1

    with history_file.open("a", encoding="utf-8") as f:
        f.write(args.commands.strip() + "\n")

    meta_file.write_text(json.dumps({"step": step_num}, indent=2), encoding="utf-8")
    step_png = session_dir / f"step_{step_num:03d}.png"

    state = run_binary(history_file, latest_png, state_json, args.show_window)
    if latest_png.exists():
        shutil.copyfile(latest_png, step_png)

    print_report(state, latest_png, step_png)
    return 0


def cmd_status(args: argparse.Namespace) -> int:
    session_dir = Path(args.session_dir).resolve()
    state_json = session_dir / "state.json"
    latest_png = session_dir / "latest.png"
    history_file = session_dir / "history.txt"

    if not state_json.exists():
        print("No active playtest session found. Run 'python3 tools/playtest.py init' first.")
        return 1

    if history_file.exists():
        print("Session Command History:")
        print(history_file.read_text(encoding="utf-8").strip())
        print("---")

    state = json.loads(state_json.read_text(encoding="utf-8"))
    print_report(state, latest_png)
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Visual Playtest Harness Controller")
    parser.add_argument(
        "--session-dir",
        default=str(DEFAULT_SESSION_DIR),
        help="Directory to store session history, screenshots, and state JSON",
    )
    parser.add_argument(
        "--show-window",
        action="store_true",
        help="Show the Raylib window during execution",
    )

    subparsers = parser.add_subparsers(dest="subcommand", required=True)

    p_init = subparsers.add_parser("init", help="Initialize a new playtest session")
    p_init.add_argument("--new-game", action="store_true", help="Automatically start a new game on the Overworld")
    p_init.set_defaults(func=cmd_init)

    p_step = subparsers.add_parser("step", help="Execute commands in the current session and capture screenshot + state")
    p_step.add_argument("commands", help="Semicolon-separated harness commands (e.g. 'move_to 200 198; face up; interact')")
    p_step.set_defaults(func=cmd_step)

    p_status = subparsers.add_parser("status", help="Show the current session state and command history")
    p_status.set_defaults(func=cmd_status)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
