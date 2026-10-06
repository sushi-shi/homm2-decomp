"""First-time setup: stage the retail images, fetch the toolchain, delink, and
configure editor tooling.

    homm2 init [--exe PATH] [--editor-exe PATH]

Each executable is verified against config/retail/targets.json and staged in
build/orig/. The scenario editor is staged when it is supplied (option or
$HOMM2_EDITOR_EXE) or already present. The delinking pipeline itself lives
under ``homm2 delink`` and is safe to rerun.
"""
import argparse
import json
import os
import subprocess
from pathlib import Path

from homm2.core.paths import REPO


def run(*cmd):
    print("[init]", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run([str(c) for c in cmd], cwd=REPO).returncode


def stage(exe: Path | None, editor_exe: Path | None) -> list[str]:
    """Stage and verify the pinned executables; write build/analysis/<key>.json."""
    from homm2.core.image import Image
    from homm2.core.inputs import read_verified, stage_executable, targets
    pins = targets(REPO)
    stage_executable(pins["game"], exe)
    selected = ["game"]
    editor = pins["editor"]
    if editor_exe is not None or os.environ.get(editor.env_var) or editor.destination.exists():
        stage_executable(editor, editor_exe)
        selected.append("editor")
    output = REPO / "build/analysis"
    output.mkdir(parents=True, exist_ok=True)
    for key in selected:
        pin = pins[key]
        report = Image(read_verified(pin, pin.destination)).report()
        path = output / f"{key}.json"
        path.write_text(json.dumps(report, indent=2) + "\n")
        print(f"[init] {key}: verified {pin.name}; report: {path.relative_to(REPO)}")
    return selected


from homm2.core.usage import logged


@logged
def main(argv=None):
    parser = argparse.ArgumentParser(prog="homm2 init", description=__doc__.splitlines()[0])
    parser.add_argument("--exe", type=Path, help="the retail HMM2PL.exe to stage")
    parser.add_argument("--editor-exe", type=Path, help="the retail EDT2PL.exe to stage")
    args = parser.parse_args(argv)
    from homm2.core.inputs import InputError
    try:
        stage(args.exe, args.editor_exe)
    except InputError as error:
        print(f"[init] {error}")
        return 1
    # Before redelink, not after: its name_strings step runs `cl` under wine, so a
    # checkout without a toolchain cannot get past it.
    from homm2.toolchain import main as toolchain
    if toolchain([]):
        return 1
    from homm2.delink.run import main as redelink
    if redelink([]):
        return 1
    if run("python3", "-m", "homm2.lsp.compdb"):
        print("[init] WARN: clangd DB step failed (editor-only; build is unaffected)")
    print("[init] done. Next: `homm2 build`")
    return 0
