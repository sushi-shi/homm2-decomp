"""Explicitly rebuild every delinker input and replace the fixed target."""

import os
import subprocess
from pathlib import Path


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[2]))


def run(*command):
    print("[redelink]", " ".join(str(item) for item in command), flush=True)
    return subprocess.run([str(item) for item in command], cwd=REPO).returncode


def main(argv=None):
    if argv:
        print("usage: homm2 redelink")
        return 1
    # Refresh public, source-private, and compiler-generated function identities.
    if run("python3", "scripts/gen_manifest.py",
           "build/orig/HEROES2W.EXE", "."):
        return 1
    if run("python3", "-m", "homm2.build.audit_text_coverage"):
        return 1
    if run("python3", "scripts/name_strings.py"):
        return 1
    if run("python3", "-m", "homm2.build.synth_pdb"):
        return 1

    # DATA/DATA_COMPGEN binding needs current candidate COFF topology.
    if run("python3", "configure.py"):
        return 1
    if run("ninja", "base"):
        return 1

    # Strict assembly precedes Vostok; target replacement is atomic.
    if run("python3", "-m", "homm2.build.reviewed_data", "--regenerate"):
        return 1
    if run("python3", "configure.py"):
        return 1
    print("[redelink] done. Next: `homm2 build`")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
