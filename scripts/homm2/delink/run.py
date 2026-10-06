"""Rebuild every delinker input from the source tree and replace the target.

The target image is stripped, so the whole inventory is project evidence:
source claims and explicit providers name identities, config/retail/functions.csv
carries the analysis candidates that fill the "(unmatched)" module, and the
reviewed manifests under config/ supply relocation sites and aliases. Candidate
objects are built first so compiler-generated providers can be re-proven before
the one symbol inventory is emitted. No previous generated inventory or
relocation-name donation participates. The pipeline is deterministic;
`homm2.delink.reviewed_data --regenerate` is a no-op when nothing changed (pass
--force to re-delink anyway).
"""

import os
import subprocess
from pathlib import Path


from homm2.core.paths import REPO


def run(*command):
    print("[redelink]", " ".join(str(item) for item in command), flush=True)
    return subprocess.run([str(item) for item in command], cwd=REPO).returncode


from homm2.core.usage import logged


@logged
def main(argv=None):
    argv = list(argv or ())
    force = "--force" in argv
    if [argument for argument in argv if argument != "--force"]:
        print("usage: homm2 redelink [--force]")
        return 1
    from homm2.core.retail import verify_retail
    try:
        verify_retail()
    except (OSError, ValueError) as error:
        print(f"[redelink] {error}")
        return 1
    if run("python3", "configure.py"):
        return 1
    from homm2.core.paths import DEFAULT_IMAGE, image_key, ninja_args
    if run("ninja", *ninja_args(), "base"):
        return 1
    if run("python3", "-m", "homm2.retail_labels.source"):
        return 1
    # Source-private identities are spelled in game addresses.
    if image_key() == DEFAULT_IMAGE and run(
            "python3", "-m", "homm2.retail_labels.annotated_functions"):
        return 1
    if run("python3", "-m", "homm2.retail_labels.name_strings"):
        return 1
    if run("python3", "-m", "homm2.delink.pdb_synth"):
        return 1
    regenerate = ["python3", "-m", "homm2.delink.reviewed_data", "--regenerate"]
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
