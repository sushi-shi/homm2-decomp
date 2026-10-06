"""Pinned, user-supplied executable inputs.

Executables are supplied explicitly and staged under ignored build/orig/
(config/retail/targets.json).
No sibling checkout or implicit source-directory search is needed.
"""
from __future__ import annotations

from dataclasses import dataclass
import hashlib
import os
from pathlib import Path
import sys
import tempfile

import json

from homm2.core.paths import REPO


class InputError(ValueError):
    """A required input is missing, unreadable, or differs from its pinned bytes."""


@dataclass(frozen=True)
class Executable:
    name: str
    destination: Path
    size: int
    sha256: str
    env_var: str
    option: str


def targets(root: Path = REPO) -> dict[str, Executable]:
    pins = json.loads((root / "config/retail/targets.json").read_text())
    return {key: Executable(value["name"], root / "build/orig" / value["name"],
                            value["size"], value["sha256"], value["env_var"],
                            value["option"]) for key, value in pins.items()}


def _verify(data: bytes, path: Path, *, size: int, sha256: str) -> None:
    if len(data) != size:
        raise InputError(f"{path}: size {len(data)} != pinned {size}")
    digest = hashlib.sha256(data).hexdigest()
    if digest != sha256:
        raise InputError(f"{path}: sha256 {digest} != pinned {sha256}")


def read_verified(executable: Executable, path: Path) -> bytes:
    try:
        data = path.read_bytes()
    except OSError as exc:
        raise InputError(f"cannot read {executable.name} at {path}: {exc}") from exc
    _verify(data, path, size=executable.size, sha256=executable.sha256)
    return data


def stage_executable(executable: Executable, source: str | Path | None = None) -> Path:
    """CLI path > environment path > verified staged copy; no implicit fallbacks.

    Validate before writing, and replace atomically so interrupted initialization
    cannot leave a partial executable. An explicit valid source can repair a bad
    staged copy; a bad explicit source is rejected even if a good copy exists.
    """
    supplied = source if source is not None else os.environ.get(executable.env_var)
    destination = executable.destination
    if supplied is None:
        if not destination.is_file():
            raise InputError(
                f"{executable.name} not initialized; set ${executable.env_var} "
                f"or run `homm2 init {executable.option} /absolute/path/to/EXE`")
        read_verified(executable, destination)
        return destination

    path = Path(supplied).expanduser().resolve()
    data = read_verified(executable, path)
    temporary = None
    try:
        if destination.is_file() and destination.read_bytes() == data:
            return destination
        destination.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(dir=destination.parent, delete=False) as fh:
            temporary = Path(fh.name)
            fh.write(data)
        temporary.replace(destination)
    except OSError as exc:
        raise InputError(f"cannot stage {executable.name} at {destination}: {exc}") from exc
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    print(f"[homm2] {executable.name} verified and copied: {path} -> {destination}",
          file=sys.stderr)
    return destination
