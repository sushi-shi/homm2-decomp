"""Provenance stamps for disposable comparison objects.

Each normalization stage writes ``<output>.stamp.json`` recording the exact
content identity of every input it consumed. A consumer of a normalized
comparison object verifies the whole recorded chain before trusting it, so a
focused raw rebuild can never be silently compared through a stale normalized
copy. A missing stamp is reported as unverifiable rather than silently
accepted; regenerating through ``homm2 build`` writes current stamps.

The stamp is diagnostic provenance for disposable comparison copies only. Raw
compiler and delinker objects remain authoritative for linking and hard gates.
"""

from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path

STAMP_SUFFIX = ".stamp.json"
STAMP_SCHEMA = 1

_HASH_CACHE: dict[str, tuple[tuple[int, int], str]] = {}


def _sha256(path: Path) -> str:
    key = str(path)
    stat = path.stat()
    identity = (stat.st_mtime_ns, stat.st_size)
    cached = _HASH_CACHE.get(key)
    if cached and cached[0] == identity:
        return cached[1]
    digest = hashlib.sha256()
    with open(path, "rb") as stream:
        for block in iter(lambda: stream.read(1 << 20), b""):
            digest.update(block)
    _HASH_CACHE[key] = (identity, digest.hexdigest())
    return _HASH_CACHE[key][1]


def stamp_path(output: Path) -> Path:
    return output.with_name(output.name + STAMP_SUFFIX)


def _portable_reference(input_path: Path, stamp_directory: Path) -> str:
    """Prefer a stamp-relative spelling so copied build trees stay coherent."""
    resolved = Path(input_path).resolve()
    try:
        return os.path.relpath(resolved, stamp_directory.resolve())
    except ValueError:
        return str(resolved)


def write_stamp(output: Path, inputs: dict[str, Path]) -> Path:
    """Record the content identity of every input consumed for ``output``."""
    path = stamp_path(output)
    payload = {
        "schema": STAMP_SCHEMA,
        "inputs": {
            role: {
                "path": _portable_reference(Path(input_path), path.parent),
                "sha256": _sha256(Path(input_path)),
            }
            for role, input_path in sorted(inputs.items())
        },
    }
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    return path


def freshness_problems(output: Path, _seen: set | None = None) -> list[str]:
    """Return every provenance problem for one normalized object.

    Each recorded input must exist and hash to its recorded identity. When a
    recorded input has its own stamp, its chain is verified recursively, so a
    rebuilt raw candidate invalidates both the paired and normalized copies
    that were derived from the old bytes.
    """
    output = Path(output)
    seen = _seen if _seen is not None else set()
    key = str(output.resolve())
    if key in seen:
        return []
    seen.add(key)
    problems = []
    stamp = stamp_path(output)
    if not stamp.is_file():
        problems.append("%s has no provenance stamp; run `homm2 build`" % output)
        return problems
    try:
        payload = json.loads(stamp.read_text())
    except (json.JSONDecodeError, OSError) as exc:
        problems.append("%s stamp is unreadable (%s); run `homm2 build`" % (output, exc))
        return problems
    if payload.get("schema") != STAMP_SCHEMA:
        problems.append("%s stamp has unknown schema; run `homm2 build`" % output)
        return problems
    for role, record in sorted(payload.get("inputs", {}).items()):
        input_path = Path(record.get("path", ""))
        if not input_path.is_absolute():
            input_path = (stamp.parent / input_path).resolve()
        if not input_path.is_file():
            problems.append("%s input %s is missing: %s" % (output, role, input_path))
            continue
        if _sha256(input_path) != record.get("sha256"):
            problems.append(
                "%s is stale: %s input changed (%s); run `homm2 build`" %
                (output, role, input_path))
            continue
        if stamp_path(input_path).is_file():
            problems.extend(freshness_problems(input_path, seen))
    return problems
