#!/usr/bin/env python3
"""homm2.core.manifest - config/units.toml, the per-TU build manifest.

This module is the ONLY place compile flags are assembled. `unit_flags` is
what ninja's compile edges receive and what every probe/permuter compile must
use; a tool that reads the raw profile list instead compiles with the wrong
flags (the /Gy tier rule bit two probe campaigns before this module existed).
"""

from __future__ import annotations

import tomllib
from pathlib import Path

from homm2.core.paths import REPO


def load(path: Path | None = None) -> dict:
    return tomllib.loads((path or REPO / "config/units.toml").read_text())


def flag_profiles(manifest: dict | None = None) -> dict[str, list[str]]:
    manifest = manifest if manifest is not None else load()
    return {name: list(flags) for name, flags in manifest.get("flags", {}).items()}


def units(manifest: dict | None = None) -> list[dict]:
    """[{unit, source, flags?}] in manifest order."""
    manifest = manifest if manifest is not None else load()
    return list(manifest.get("unit", []))


def unit_flags(unit: dict, manifest: dict | None = None) -> list[str]:
    """The complete compile flags for one manifest unit row.

    Retail linked BASE as a function-packaged static library: /Gy supplies the
    per-function 16-byte section boundaries visible at every BASE VA, while
    the explicit SOURCE (and EDITOR) objects remain un-packaged. That tier
    rule lives here and nowhere else.
    """
    profiles = flag_profiles(manifest)
    flags = list(profiles[unit.get("flags", "base")])
    if unit["unit"].startswith("BASE/"):
        flags.append("/Gy")
    return flags
