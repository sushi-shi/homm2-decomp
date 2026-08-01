"""homm2.core.paths - the single repository-root resolver.

Tools live at several depths under ``scripts/`` and must not each re-derive the
root by counting their own nesting. A module written as ``parents[1]`` is correct
exactly where it was born; move it one package deeper and it silently resolves to
``scripts/`` instead, then reads and writes the wrong tree without failing.

Resolution order, most explicit first:

1. ``HOMM2_DIR`` - exported by the dev shell (``flake.nix``) and by the
   toolchain-release sandbox, where the sources run from the Nix store and no
   ancestor directory belongs to the repository at all.
2. the nearest ancestor holding ``flake.nix`` - correct at any depth, so moving a
   module between packages needs no edit here.
3. the working directory - last resort.
"""
import os
from pathlib import Path


def find_repo(start=None):
    """Return the repository root for a module living at any depth."""
    override = os.environ.get("HOMM2_DIR")
    if override:
        return Path(override).resolve()
    origin = Path(start).resolve() if start else Path(__file__).resolve()
    for parent in origin.parents:
        if (parent / "flake.nix").exists():
            return parent
    return Path.cwd().resolve()


REPO = find_repo()
