"""Generate and keep fresh reviewed public-data delinker targets.

NB09 public-data records prove names, addresses, and compiland ownership, but do not
carry allocation lengths.  Exact extents in this module always come from the reviewed
initialized-storage ledger; the provisional next-public gaps in symbol_names.csv are
never used as sizes.
"""
import argparse
import csv
import hashlib
import json
import os
import shutil
import subprocess
import tempfile
from pathlib import Path

from homm2.build.link_exe import (
    classify_pe_storage,
    load_required_initialized_storage,
    read_pe,
    read_pe_payload_evidence,
)


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SYMBOLS = REPO / "build/gen/symbol_names.csv"
LEDGER = REPO / "config/required_initialized_storage.tsv"
EXE = REPO / "build/orig/HEROES2W.EXE"
PDB = REPO / "build/pdb/HEROES2W.pdb"
MANIFEST = REPO / "build/gen/reviewed_delink_data.tsv"
TARGET = REPO / "build/delink"
STAMP = TARGET / ".reviewed-data-stamp.json"


def _digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def reviewed_manifest_bytes(symbols=SYMBOLS, ledger=LEDGER, exe=EXE):
    with Path(symbols).open(newline="", encoding="latin-1") as stream:
        inventory = {row["name"]: row for row in csv.DictReader(stream)}
    retail = read_pe(exe)
    rows = []
    for required in load_required_initialized_storage(ledger):
        symbol = inventory.get(required["name"])
        if symbol is None:
            raise RuntimeError("reviewed data is absent from public inventory: %s" %
                               required["name"])
        if symbol.get("kind") != "data" or symbol.get("unit") != required["unit"]:
            raise RuntimeError("reviewed data public kind/owner mismatch: %s" %
                               required["name"])
        provenance = symbol.get("provenance")
        if provenance and provenance != "cv-public-data":
            raise RuntimeError("reviewed data lacks CV public-data provenance: %s" %
                               required["name"])
        rva = int(symbol["rva"], 0)
        storage = classify_pe_storage(retail, rva)["class"]
        if storage != "data-initialized":
            raise RuntimeError("reviewed initialized data has retail storage %s: %s" %
                               (storage, required["name"]))
        evidence = read_pe_payload_evidence(exe, rva, required["size"], required["audit"])
        if (evidence["sha256"] != required["retail_sha256"] or
                evidence["highlow_base_relocation_count"] != required["highlow_count"]):
            raise RuntimeError("reviewed retail evidence changed: %s" % required["name"])
        rows.append((required["unit"], rva, required["name"], required["size"]))
    lines = [
        "# Sizes are reviewed required_initialized_storage.tsv evidence, not NB09 lengths.",
        "name\tobject\trva\tsize\tstorage\talignment\tprovenance",
    ]
    for unit, rva, name, size in sorted(rows):
        object_name = unit.replace("/", "\\") + ".c"
        lines.append("%s\t%s\t0x%x\t0x%x\tdata\t0x4\treviewed-required-initialized-storage" %
                     (name, object_name, rva, size))
    return ("\n".join(lines) + "\n").encode("utf-8")


def _identity_inputs(delinker):
    return {
        "schema": 1,
        "ledger_sha256": _digest(LEDGER),
        "symbols_sha256": _digest(SYMBOLS),
        "exe_sha256": _digest(EXE),
        "pdb_sha256": _digest(PDB),
        "delinker_sha256": _digest(delinker),
        "adapter_sha256": _digest(__file__),
    }


def _identity(manifest, delinker):
    return {
        **_identity_inputs(delinker),
        "manifest_sha256": hashlib.sha256(manifest).hexdigest(),
    }


def refresh_required(current, expected):
    return current is None or any(current.get(key) != value
                                  for key, value in expected.items())


def _expected_objects(manifest):
    return {line.split("\t")[1].replace("\\", "/") + ".obj"
            for line in manifest.decode().splitlines()
            if line and not line.startswith("#") and not line.startswith("name\t")}


def _validate_owner_objects(root, manifest):
    missing = [name for name in sorted(_expected_objects(manifest))
               if not (Path(root) / name).is_file()]
    if missing:
        raise RuntimeError("reviewed delink omitted owner objects: %s" % ", ".join(missing))


def _refresh_objdiff_targets():
    path = REPO / "build/objdiff/objdiff.json"
    if not path.exists():
        return
    config = json.loads(path.read_text())
    changed = False
    for unit in config.get("units", []):
        target = TARGET / (unit["name"] + ".c.obj")
        expected = "../delink/%s.c.obj" % unit["name"]
        if target.exists() and unit.get("target_path") != expected:
            unit["target_path"] = expected
            changed = True
    if changed:
        path.write_text(json.dumps(config, indent=2) + "\n")


def ensure_reviewed_targets(delinker=None):
    """Refresh only delinked target objects when reviewed data evidence changes."""
    delinker = Path(delinker or shutil.which("vostok-delinker") or "")
    def display_path(path):
        try:
            return str(path.relative_to(REPO))
        except ValueError:
            return str(path)

    missing = [display_path(path) for path in (EXE, PDB) if not path.is_file()]
    if missing:
        raise RuntimeError(
            "reviewed target refresh requires %s; run `homm2 init`" %
            ", ".join(missing))
    if not delinker.is_file():
        raise RuntimeError("vostok-delinker is required to validate reviewed target freshness")
    try:
        current = json.loads(STAMP.read_text())
    except (FileNotFoundError, json.JSONDecodeError):
        current = None
    if not refresh_required(current, _identity_inputs(delinker)):
        _refresh_objdiff_targets()
        return False
    manifest = reviewed_manifest_bytes()
    identity = _identity(manifest, delinker)

    MANIFEST.parent.mkdir(parents=True, exist_ok=True)
    MANIFEST.write_bytes(manifest)
    TARGET.parent.mkdir(parents=True, exist_ok=True)
    temporary = Path(tempfile.mkdtemp(prefix="delink-reviewed-", dir=TARGET.parent))
    backup = TARGET.with_name(TARGET.name + ".pre-reviewed")
    try:
        subprocess.run([
            str(delinker), "--pdb-path", str(PDB), "--exe-path", str(EXE),
            "--output-path", str(temporary), "--engine-path", "c:\\proj\\",
            "--data-manifest", str(MANIFEST),
        ], cwd=REPO, check=True)
        _validate_owner_objects(temporary, manifest)
        (temporary / STAMP.name).write_text(json.dumps(identity, indent=2) + "\n")
        if backup.exists():
            shutil.rmtree(backup)
        if TARGET.exists():
            TARGET.rename(backup)
        temporary.rename(TARGET)
        shutil.rmtree(backup, ignore_errors=True)
    except BaseException:
        if backup.exists() and not TARGET.exists():
            backup.rename(TARGET)
        shutil.rmtree(temporary, ignore_errors=True)
        raise
    _refresh_objdiff_targets()
    return True


def record_current_targets(delinker=None):
    """Stamp a target tree just emitted by init without running the delinker again."""
    delinker = Path(delinker or shutil.which("vostok-delinker") or "")
    if not delinker.is_file():
        raise RuntimeError("vostok-delinker is required to record target freshness")
    manifest = reviewed_manifest_bytes()
    if not MANIFEST.is_file() or MANIFEST.read_bytes() != manifest:
        raise RuntimeError("generated reviewed data manifest is stale")
    _validate_owner_objects(TARGET, manifest)
    STAMP.write_text(json.dumps(_identity(manifest, delinker), indent=2) + "\n")


def main(argv=None):
    parser = argparse.ArgumentParser()
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--write-manifest", action="store_true")
    action.add_argument("--ensure", action="store_true")
    action.add_argument("--record-current", action="store_true")
    args = parser.parse_args(argv)
    if args.write_manifest:
        MANIFEST.parent.mkdir(parents=True, exist_ok=True)
        MANIFEST.write_bytes(reviewed_manifest_bytes())
    elif args.ensure:
        ensure_reviewed_targets()
    else:
        record_current_targets()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
