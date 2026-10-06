#!/usr/bin/env python3
"""homm2.manifest - config/units.toml, the per-TU build manifest.

A [[unit]] links into the images its `images` list names (default: the game
only). `units()` answers for the selected image (`homm2 --image`), so every
consumer sees exactly the units of the program it is reconstructing;
`all_units()` is the whole manifest. An image other than the game may add
compile defines under `[images.<key>]`; a shared source then compiles once per
image with that image's context.

This module is the ONLY place compile flags are assembled. `unit_flags` is
what ninja's compile edges receive and what every probe/permuter compile must
use; a tool that reads the raw profile list instead compiles with the wrong
flags (the /Gy tier rule bit two probe campaigns before this module existed).
"""

from __future__ import annotations

import tomllib
from pathlib import Path

from homm2.core.paths import DEFAULT_IMAGE, REPO, image_key


def load(path: Path | None = None) -> dict:
    return tomllib.loads((path or REPO / "config/units.toml").read_text())


def flag_profiles(manifest: dict | None = None) -> dict[str, list[str]]:
    manifest = manifest if manifest is not None else load()
    return {name: list(flags) for name, flags in manifest.get("flags", {}).items()}


def unit_images(unit: dict) -> list[str]:
    """The images a manifest unit links into."""
    return list(unit.get("images", [DEFAULT_IMAGE]))


def all_units(manifest: dict | None = None) -> list[dict]:
    """Every [[unit]] in manifest order, whatever its images."""
    manifest = manifest if manifest is not None else load()
    return list(manifest.get("unit", []))


def units(manifest: dict | None = None, image: str | None = None) -> list[dict]:
    """[{unit, source, flags?}] of the selected image, in manifest order."""
    key = image or image_key()
    return [u for u in all_units(manifest) if key in unit_images(u)]


def image_defines(image: str | None = None, manifest: dict | None = None) -> list[str]:
    """`/D` flags the image adds to every unit it compiles (none for the game)."""
    manifest = manifest if manifest is not None else load()
    table = manifest.get("images", {}).get(image or image_key(), {})
    return [f"/D{d}" for d in table.get("defines", [])]


def unit_flags(unit: dict, manifest: dict | None = None,
               image: str | None = None) -> list[str]:
    """The complete compile flags for one manifest unit row.

    Retail linked BASE as a function-packaged static library: /Gy supplies the
    per-function 16-byte section boundaries visible at every BASE VA, while
    the explicit SOURCE (and EDITOR) objects remain un-packaged. The BASE
    library also used VC6's automatic precompiled headers (/YX): header-inline
    functions and a header class's deleting destructor are then emitted at the
    end of the object, which is the retail DIMMER and AudiereEffects order.
    /YX is byte-neutral for every other unit (measured on all C++ units). That
    tier rule lives here and nowhere else.
    """
    manifest = manifest if manifest is not None else load()
    profiles = flag_profiles(manifest)
    key = image or image_key()
    profile = unit.get("image_flags", {}).get(key, unit.get("flags", "base"))
    flags = list(profiles[profile])
    if unit["unit"].startswith("BASE/"):
        flags.extend(("/Gy", "/YX"))
    return flags + image_defines(key, manifest)


def claim_files(source_root: Path | None = None, image: str | None = None,
                pattern: str = "*.cpp") -> list[Path]:
    """The source files whose `VA`/`DATA`/`VTBL` markers spell the image's
    addresses (its claim space), in sorted order.

    A unit linked into the game spells game addresses, even when another image
    links it too (that image reads its identities through placements). A unit
    linked only into other images spells that image's addresses. A tree other
    than the repository's src/ (a fixture, a clean export) is returned whole.
    """
    root = Path(source_root) if source_root is not None else REPO / "src"
    files = sorted(root.rglob(pattern))
    if root.resolve() != (REPO / "src").resolve():
        return files
    key = image or image_key()
    owner = {(REPO / u["source"]).resolve(): unit_images(u) for u in all_units()}

    def ours(path: Path) -> bool:
        images = owner.get(path.resolve())
        if images is None:                  # not a manifest unit: a game-tree file
            return key == DEFAULT_IMAGE
        if key == DEFAULT_IMAGE:
            return DEFAULT_IMAGE in images
        return key in images and DEFAULT_IMAGE not in images
    return [path for path in files if ours(path)]
