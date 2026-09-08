#!/usr/bin/env python3
"""Exercise the real Linux game with private retail data and a private seed save."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

from package_web_data import resolve_name


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def run(args: argparse.Namespace) -> None:
    data = args.data.resolve(strict=True)
    seed = args.seed.resolve(strict=True)
    # Preserve executable symlink names: multicall wrappers can depend on argv[0].
    executables = [path.absolute() for path in args.exe]
    for executable in executables:
        if not executable.is_file() or not os.access(executable, os.X_OK):
            raise ValueError(f"not an executable file: {executable}")
    if len(executables) == 1:
        executables *= 2
    output = args.output.absolute()
    # Refuse to overwrite previous evidence or link an installed save directory.
    output.mkdir(parents=True, exist_ok=False)
    for name in ("DATA", "MAPS", "MUSIC", "Tracks2", "HEROES2", "HELP",
                 "H2CAMP.TXT", "POLCAMP.TXT"):
        source = resolve_name(data, name)
        if source is not None:
            (output / name).symlink_to(source.resolve(), target_is_directory=source.is_dir())
    saves = output / "state/homm2/homm2/GAMES"
    saves.mkdir(parents=True)
    save = saves / "NEWGAME.GM1"
    shutil.copyfile(seed, save)
    original = save.read_bytes()
    if not original:
        raise ValueError("the seed save is empty")
    replay = Path(__file__).with_name("replays") / "save-roundtrip.replay"
    env = os.environ | {
        "SDL_VIDEODRIVER": "dummy", "SDL_AUDIODRIVER": "dummy",
        "SDL_RENDER_DRIVER": "software", "HOMM2_DATA": str(output),
        "XDG_DATA_HOME": str(output / "state"),
        "HOMM2_INPUT_REPLAY": str(replay), "HOMM2_SCREENSHOT_EVERY": "30",
        "ASAN_OPTIONS": "halt_on_error=1:detect_leaks=1",
        "UBSAN_OPTIONS": "halt_on_error=1:print_stacktrace=1",
    }
    report = {"seed_sha256": digest(original), "cycles": []}
    previous = None
    for cycle, executable in enumerate(executables, 1):
        before = save.stat()
        prefix = f"cycle-{cycle}"
        log = output / f"{prefix}.log"
        env["HOMM2_SCREENSHOT"] = str(output / prefix)
        with log.open("w") as stream:
            process = subprocess.run([str(executable), "/i0"], cwd=output, env=env,
                                     stdout=stream, stderr=subprocess.STDOUT,
                                     timeout=args.timeout, check=False)
        text = log.read_text(errors="replace")
        if process.returncode != 0:
            raise RuntimeError(f"cycle {cycle}: game exited {process.returncode}; see {log}")
        if "localization: save-text=" not in text:
            raise RuntimeError(f"cycle {cycle}: no successful save-load observation; see {log}")
        if "runtime error:" in text or "ERROR: AddressSanitizer" in text:
            raise RuntimeError(f"cycle {cycle}: sanitizer diagnostic; see {log}")
        after = save.stat()
        if after.st_ino == before.st_ino or after.st_mtime_ns <= before.st_mtime_ns:
            raise RuntimeError(f"cycle {cycle}: the game did not replace the save")
        payload = save.read_bytes()
        if not payload or (previous is not None and payload != previous):
            raise RuntimeError(f"cycle {cycle}: save bytes changed across a fresh-process round trip")
        if args.expect_seed_unchanged and payload != original:
            raise RuntimeError(f"cycle {cycle}: save differs from the expected seed bytes")
        frames = sorted(output.glob(f"{prefix}.*.ppm"))
        hashes = []
        for frame in frames:
            pixels = frame.read_bytes()
            header = b"P6\n640 480\n255\n"
            if not pixels.startswith(header) or len(pixels) != len(header) + 640 * 480 * 3:
                raise RuntimeError(f"invalid framebuffer capture: {frame}")
            hashes.append(digest(pixels))
        if len(set(hashes)) < 2:
            raise RuntimeError(f"cycle {cycle}: missing distinct rendered frames")
        shutil.copyfile(save, output / f"{prefix}.GM1")
        report["cycles"].append({"executable": str(executable), "save_bytes": len(payload),
                                 "save_sha256": digest(payload), "frames": len(frames),
                                 "frame_sha256": hashes, "replaced": True})
        (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
        print(f"cycle {cycle}: replaced {len(payload)}-byte save, sha256={digest(payload)}, "
              f"{len(frames)} frames", flush=True)
        previous = payload


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, action="append", required=True,
                        help="Linux game binary; repeat to exchange one save across builds")
    parser.add_argument("--data", type=Path, required=True, help="installed retail game directory")
    parser.add_argument("--seed", type=Path, required=True, help="normal, playable .GM1 save")
    parser.add_argument("--output", type=Path, required=True, help="new directory for private evidence")
    parser.add_argument("--timeout", type=float, default=65, help="seconds per game process")
    parser.add_argument("--expect-seed-unchanged", action="store_true",
                        help="also compare every result with an already normalized NEWGAME.GM1 seed")
    args = parser.parse_args()
    try:
        run(args)
    except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(f"gameplay round trip failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
