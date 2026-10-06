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

IMAGES. The repository reconstructs more than one linked program
(config/retail/targets.json: ``game`` = HMM2PL.exe, ``editor`` = EDT2PL.exe).
Every authoritative fact is keyed by (image, rva): the selected image
(``homm2 --image KEY``, exported to children as ``$HOMM2_IMAGE``; default
``game``) decides which retail executable, which config/retail table set and
which generated trees a command reads. The game keeps config/retail/ and
build/; another image's state lives under config/retail/<image>/ and
build/<image>/. ``RETAIL`` and ``IMAGE_BUILD`` resolve lazily to the selected
image's table directory and generated-state root; ``RETAIL_ROOT`` and
``BUILD`` are shared.
"""
from __future__ import annotations

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
CONFIG = REPO / "config"
RETAIL_ROOT = CONFIG / "retail"
BUILD = REPO / "build"

#: Environment variable carrying the selected image to every child process.
IMAGE_ENV = "HOMM2_IMAGE"
DEFAULT_IMAGE = "game"


def images() -> list[str]:
    """The pinned image keys, in targets.json order (the game first)."""
    import json
    return list(json.loads((RETAIL_ROOT / "targets.json").read_text()))


def image_key() -> str:
    """The selected image; an unknown key is an error, never a fallback."""
    key = os.environ.get(IMAGE_ENV) or DEFAULT_IMAGE
    if key not in images():
        raise RuntimeError(f"${IMAGE_ENV}={key!r} is not a pinned image "
                           f"(config/retail/targets.json: {images()})")
    return key


def retail_dir(image: str | None = None) -> Path:
    """config/retail for the game; config/retail/<image> for another image."""
    key = image or image_key()
    return RETAIL_ROOT if key == DEFAULT_IMAGE else RETAIL_ROOT / key


def image_build(image: str | None = None) -> Path:
    """Root of the image's generated state: build/ for the game,
    build/<image>/ for another image (toolchains and the Wine prefix stay
    shared)."""
    key = image or image_key()
    return BUILD if key == DEFAULT_IMAGE else BUILD / key


def gen_dir(image: str | None = None) -> Path:
    return image_build(image) / "gen"


def objdiff_dir(image: str | None = None) -> Path:
    """Delinked targets, candidate objects and reports of one image."""
    return image_build(image) / "objdiff"


def delink_dir(image: str | None = None) -> Path:
    return image_build(image) / "delink"


def retail_exe(image: str | None = None) -> Path:
    """The staged retail executable of the image (its pin's destination, or
    the path its environment variable names)."""
    from homm2.core.inputs import targets
    pin = targets(REPO)[image or image_key()]
    return Path(os.environ.get(pin.env_var) or pin.destination)


def image_paths(image: str | None = None):
    """Repository-relative spellings of the image's roots, for generated build
    files: `build` (generated state), `retail` (config tables), `exe` (the
    staged retail executable) and `env` (the shell prefix that selects the
    image for a child; empty for the game)."""
    from types import SimpleNamespace
    key = image or image_key()
    return SimpleNamespace(
        key=key,
        build=image_build(key).relative_to(REPO).as_posix(),
        retail=retail_dir(key).relative_to(REPO).as_posix(),
        exe=retail_exe(key).relative_to(REPO).as_posix()
        if retail_exe(key).is_relative_to(REPO) else str(retail_exe(key)),
        env="" if key == DEFAULT_IMAGE else f"{IMAGE_ENV}={key} ")


def __getattr__(name: str):
    # `from homm2.core.paths import RETAIL` binds the selected image's table
    # directory (IMAGE_BUILD: its generated-state root) at import time, after
    # the CLI has selected the image.
    if name == "RETAIL":
        return retail_dir()
    if name == "IMAGE_BUILD":
        return image_build()
    raise AttributeError(name)


def ninja_args(image: str | None = None) -> list[str]:
    """`ninja` arguments for the selected image: its graph file (the game's is
    the root build.ninja) and the $HOMM2_JOBS cap."""
    key = image or image_key()
    graph = [] if key == DEFAULT_IMAGE else [
        "-f", (image_build(key) / "build.ninja").relative_to(REPO).as_posix()]
    return graph + ninja_jobs()


def ninja_jobs() -> list[str]:
    """`-j N` from $HOMM2_JOBS (ninja's own default when unset), so a shared
    machine can cap every ninja the tooling starts."""
    jobs = os.environ.get("HOMM2_JOBS", "").strip()
    return ["-j", jobs] if jobs.isdigit() and int(jobs) > 0 else []
