"""Rebuild every delinker input from the source tree and replace the target.

The target image is stripped, so the whole inventory is project evidence:
source VA/DATA markers name what has been claimed, config/retail_functions.csv
carries the analysis candidates that fill the "(unmatched)" module, and the
reviewed manifests under config/ supply relocation sites and aliases. The
pipeline is deterministic; `homm2.build.reviewed_data --regenerate` is a no-op
when nothing changed (pass --force to re-delink anyway).
"""

import os
import subprocess
from pathlib import Path


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[2]))


def run(*command):
    print("[redelink]", " ".join(str(item) for item in command), flush=True)
    return subprocess.run([str(item) for item in command], cwd=REPO).returncode


def main(argv=None):
    argv = list(argv or ())
    force = "--force" in argv
    if [argument for argument in argv if argument != "--force"]:
        print("usage: homm2 redelink [--force]")
        return 1
    if run("python3", "-m", "homm2.build.source_symbols"):
        return 1
    if run("python3", "-m", "homm2.build.name_strings"):
        return 1
    if run("python3", "-m", "homm2.build.synth_pdb"):
        return 1
    regenerate = ["python3", "-m", "homm2.build.reviewed_data", "--regenerate"]
    if force:
        regenerate.append("--force")
    if run(*regenerate):
        return 1
    # Reconfigure last so the ninja graph and objdiff pairing see the fresh
    # target set (a newly claimed unit's <unit>.c.obj, or "(unmatched)").
    if run("python3", "configure.py"):
        return 1
    print("[redelink] done. Next: `homm2 build`")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
