"""Regenerate and validate the delinked matching target.

The retail image is stripped: no debug stream names a symbol and no base-
relocation directory lists a DIR32 site.  Everything the delinker consumes is
project evidence - the source-marker inventory (build/gen/symbol_names.csv),
the PDB synthesized from it, and the reviewed site/alias manifests under
config/.  Regeneration is deterministic and idempotent: rerunning it with
unchanged inputs is a no-op, and every build validates the stamp against the
same identity, so a stale target is refused rather than silently compared.
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

from homm2.build.data_manifest_adapter import (
    DELINK_HEADER,
    candidate_common_manifest_bytes,
    candidate_section_manifest_bytes,
    delinker_manifest_bytes,
    render_source_manifest,
    source_manifest_rows,
)
from homm2.build.link_exe import (
    classify_pe_storage,
    load_required_initialized_storage,
    read_pe,
    read_pe_payload_evidence,
)
from homm2.build.reloc_owners import (
    OWNER_EXTENTS, load_definition_rvas, load_explicit_extents,
    load_owner_ranges,
)


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SYMBOLS = REPO / "build/gen/symbol_names.csv"
RETAIL_FUNCTIONS = REPO / "config/retail_functions.csv"
LEDGER = REPO / "config/required_initialized_storage.tsv"
EXE = REPO / "build/orig/HMM2PL.exe"
PDB = REPO / "build/pdb/HMM2PL.pdb"
RELOC_MANIFEST = REPO / "config/delink_relocs.tsv"
RELOC_ALIASES = REPO / "config/delink_reloc_aliases.tsv"
MANIFEST = REPO / "build/gen/reviewed_delink_data.tsv"
DATA_MANIFEST = REPO / "build/gen/delink_data_manifest.tsv"
SOURCE_DATA_MANIFEST = REPO / "build/gen/delink_data_from_source.tsv"
DATA_SECTION_MANIFEST = REPO / "build/gen/delink_data_sections.tsv"
COMMON_MANIFEST = REPO / "build/gen/delink_common_symbols.tsv"
TARGET = REPO / "build/delink"
STAMP = TARGET / ".reviewed-data-stamp.json"
DATA_ADAPTER = Path(__file__).with_name("data_manifest_adapter.py")


def _digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def reviewed_manifest_bytes(symbols=SYMBOLS, ledger=LEDGER, exe=EXE):
    """Reviewed initialized-storage rows, each proven against retail bytes."""
    with Path(symbols).open(newline="", encoding="latin-1") as stream:
        inventory = {row["name"]: row for row in csv.DictReader(stream)}
    retail = read_pe(exe)
    rows = {}
    for required in load_required_initialized_storage(ledger):
        symbol = inventory.get(required["name"])
        if symbol is None:
            raise RuntimeError("reviewed data is absent from the source inventory: %s" %
                               required["name"])
        if symbol.get("kind") != "data" or symbol.get("unit") != required["unit"]:
            raise RuntimeError("reviewed data kind/owner mismatch: %s" %
                               required["name"])
        rva = int(symbol["rva"], 0)
        start_storage = classify_pe_storage(retail, rva)["class"]
        end_storage = classify_pe_storage(
            retail, rva + required["size"] - 1)["class"]
        if start_storage != "data-initialized" or end_storage != "data-initialized":
            raise RuntimeError(
                "reviewed initialized data crosses retail storage %s -> %s: %s" %
                (start_storage, end_storage, required["name"]))
        evidence = read_pe_payload_evidence(exe, rva, required["size"], required["audit"])
        if (evidence["sha256"] != required["retail_sha256"] or
                evidence["highlow_base_relocation_count"] != required["highlow_count"]):
            raise RuntimeError("reviewed retail evidence changed: %s" % required["name"])
        rows[required["name"]] = {
            "unit": required["unit"], "rva": rva,
            "name": required["name"], "size": required["size"],
            "storage": "data", "provenance": "reviewed-required-initialized-storage",
        }

    storage_names = {
        "rdata": "rdata",
        "data-initialized": "data",
        "data-loader-zero-tail": "bss",
    }
    for owner in load_owner_ranges(symbols_path=str(symbols)):
        symbol = inventory.get(owner.symbol)
        if symbol is None or symbol.get("kind") != "data":
            raise RuntimeError("reviewed owner is absent from the source inventory: %s" %
                               owner.symbol)
        if int(symbol["rva"], 0) != owner.rva:
            raise RuntimeError("reviewed owner RVA mismatch: %s" % owner.symbol)
        start = classify_pe_storage(retail, owner.rva)["class"]
        end = classify_pe_storage(retail, owner.rva + owner.size - 1)["class"]
        if start != end or start not in storage_names:
            raise RuntimeError(
                "reviewed owner crosses unsupported retail storage %s -> %s: %s" %
                (start, end, owner.symbol))
        record = {
            "unit": symbol["unit"], "rva": owner.rva,
            "name": owner.symbol, "size": owner.size,
            "storage": storage_names[start], "provenance": "reviewed-public-owner-extent",
        }
        previous = rows.get(owner.symbol)
        if previous is not None:
            if (previous["unit"], previous["rva"], previous["size"]) != (
                    record["unit"], record["rva"], record["size"]):
                raise RuntimeError(
                    "reviewed allocation and owner extent disagree: %s" % owner.symbol)
            continue
        rows[owner.symbol] = record

    lines = [
        "# Reviewed allocations proven against retail bytes and reviewed sites.",
        "name\tobject\trva\tsize\tstorage\tprovenance",
    ]
    for row in sorted(rows.values(), key=lambda value: (
            value["unit"], value["storage"], value["rva"])):
        object_name = row["unit"].replace("/", "\\") + ".c"
        lines.append("%s\t%s\t0x%x\t0x%x\t%s\t%s" % (
            row["name"], object_name, row["rva"], row["size"],
            row["storage"], row["provenance"]))
    return ("\n".join(lines) + "\n").encode("utf-8")


def data_manifest_stub_bytes():
    """Header-only delinker data manifest until reviewed data identities exist.

    The data campaign will replace this with rows bound from DATA() markers and
    candidate COFF topology; every normalize edge already reads the file, so it
    must exist with the adapter's exact header from the first delink on.
    """
    return ("# No reviewed data identities yet; rows appear when the data "
            "campaign binds DATA() markers to candidate COFF topology.\n"
            + "\t".join(DELINK_HEADER) + "\n").encode("utf-8")


def _identity_inputs(delinker):
    definitions = load_definition_rvas()
    owner_definitions = "\n".join(
        "%s=0x%x" % (name, definitions.get(name, -1))
        for name in sorted(load_explicit_extents()))
    return {
        "schema": 13,
        "exe_sha256": _digest(EXE),
        "pdb_sha256": _digest(PDB),
        "symbols_sha256": _digest(SYMBOLS),
        "retail_functions_sha256": _digest(RETAIL_FUNCTIONS),
        "reloc_manifest_sha256": _digest(RELOC_MANIFEST),
        "reloc_aliases_sha256": _digest(RELOC_ALIASES),
        "ledger_sha256": _digest(LEDGER),
        "owner_extents_sha256": _digest(REPO / OWNER_EXTENTS),
        "owner_definitions_sha256": hashlib.sha256(
            owner_definitions.encode("utf-8")).hexdigest(),
        "manifest_sha256": hashlib.sha256(reviewed_manifest_bytes()).hexdigest(),
        "source_data_manifest_sha256": _digest(SOURCE_DATA_MANIFEST),
        "data_manifest_sha256": _digest(DATA_MANIFEST),
        "data_section_manifest_sha256": _digest(DATA_SECTION_MANIFEST),
        "common_manifest_sha256": _digest(COMMON_MANIFEST),
        "delinker_sha256": _digest(delinker),
        "generator_sha256": _digest(__file__),
        "data_adapter_sha256": _digest(DATA_ADAPTER),
    }


def _identity_input_files():
    return (EXE, PDB, SYMBOLS, RETAIL_FUNCTIONS, RELOC_MANIFEST, RELOC_ALIASES,
            LEDGER, REPO / OWNER_EXTENTS, SOURCE_DATA_MANIFEST, DATA_MANIFEST,
            DATA_SECTION_MANIFEST,
            COMMON_MANIFEST,
            )


def _resolve_delinker(delinker=None):
    delinker = Path(delinker or shutil.which("vostok-delinker") or "")
    if not delinker.is_file():
        raise RuntimeError(
            "vostok-delinker is required; enter `nix develop .#build`")
    return delinker


def _expected_delink_objects(symbols=None, retail_functions=None):
    """Object names the delinker must emit for the current inventories."""
    symbols = Path(symbols or SYMBOLS)
    retail_functions = Path(retail_functions or RETAIL_FUNCTIONS)
    claimed_units = set()
    claimed_rvas = set()
    with Path(symbols).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                claimed_units.add(row["unit"])
                claimed_rvas.add(int(row["rva"], 0))
    expected = {unit + ".c.obj" for unit in claimed_units}
    with Path(retail_functions).open(newline="") as stream:
        rows = csv.DictReader(
            line for line in stream if not line.lstrip().startswith("#"))
        if any(int(row["entry_rva"], 16) not in claimed_rvas for row in rows):
            expected.add("(unmatched).c.obj")
    return expected


def _atomic_write(path, payload):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    handle, temporary = tempfile.mkstemp(prefix=".%s." % path.name, dir=path.parent)
    try:
        with os.fdopen(handle, "wb") as stream:
            stream.write(payload)
        os.replace(temporary, path)
    except BaseException:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass
        raise


def regenerate_targets(delinker=None, force=False):
    """Delink the retail image into target objects and replace them atomically."""
    generation_inputs = (
        EXE, PDB, SYMBOLS, RETAIL_FUNCTIONS, RELOC_MANIFEST, RELOC_ALIASES,
        LEDGER, REPO / OWNER_EXTENTS,
    )
    missing = [path for path in generation_inputs if not path.is_file()]
    if missing:
        raise RuntimeError("delink regeneration requires %s" %
                           ", ".join(str(path) for path in missing))
    delinker = _resolve_delinker(delinker)
    _atomic_write(MANIFEST, reviewed_manifest_bytes())
    source_rows, diagnostics = source_manifest_rows(strict=False)
    _atomic_write(SOURCE_DATA_MANIFEST, render_source_manifest(source_rows))
    _atomic_write(DATA_MANIFEST, delinker_manifest_bytes(source_rows))
    _atomic_write(DATA_SECTION_MANIFEST, candidate_section_manifest_bytes())
    _atomic_write(COMMON_MANIFEST, candidate_common_manifest_bytes())
    for diagnostic in diagnostics:
        print(f"[reviewed-data] OPEN {diagnostic}")
    identity = _identity_inputs(delinker)
    if not force:
        try:
            if json.loads(STAMP.read_text()) == identity:
                print("[reviewed-data] delinked target is current; nothing to do "
                      "(--force re-delinks anyway)")
                return
        except (FileNotFoundError, json.JSONDecodeError):
            pass
    TARGET.parent.mkdir(parents=True, exist_ok=True)
    temporary = Path(tempfile.mkdtemp(prefix="delink-", dir=TARGET.parent))
    backup = TARGET.with_name(TARGET.name + ".pre-delink")
    try:
        # The image ships no .reloc directory, so every DIR32 site the delinker
        # may use comes from the reviewed config/delink_relocs.tsv; the alias
        # manifest resolves reviewed interior references to their owner symbol.
        subprocess.run([
            str(delinker), "--pdb-path", str(PDB), "--exe-path", str(EXE),
            "--output-path", str(temporary), "--engine-path", "c:\\proj\\",
            "--data-manifest", str(DATA_MANIFEST),
            "--data-section-manifest", str(DATA_SECTION_MANIFEST),
            "--common-symbol-manifest", str(COMMON_MANIFEST),
            "--reloc-manifest", str(RELOC_MANIFEST),
            "--reloc-alias-manifest", str(RELOC_ALIASES),
        ], cwd=REPO, check=True)
        absent = [name for name in sorted(_expected_delink_objects())
                  if not (temporary / name).is_file()]
        if absent:
            raise RuntimeError("delink omitted expected objects: %s" %
                               ", ".join(absent))
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


def ensure_reviewed_targets(delinker=None):
    """Validate the fixed target against current inputs without changing it."""
    if not TARGET.is_dir():
        raise RuntimeError("delinked target is missing; run `homm2 redelink`")
    try:
        current = json.loads(STAMP.read_text())
    except (FileNotFoundError, json.JSONDecodeError):
        raise RuntimeError(
            "delinked target has no valid provenance stamp; run `homm2 redelink`")
    missing = [path for path in _identity_input_files() if not path.is_file()]
    if missing:
        raise RuntimeError("delink identity inputs are missing (%s); "
                           "run `homm2 redelink`" %
                           ", ".join(str(path) for path in missing))
    expected = _identity_inputs(_resolve_delinker(delinker))
    if current != expected:
        stale = sorted(key for key in expected
                       if current.get(key) != expected[key])
        raise RuntimeError("delinked target is stale (%s changed); "
                           "run `homm2 redelink`" % ", ".join(stale))
    return False


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--regenerate", action="store_true")
    action.add_argument("--ensure", action="store_true")
    parser.add_argument("--force", action="store_true",
                        help="re-delink even when the stamp already matches")
    args = parser.parse_args(argv)
    if args.regenerate:
        regenerate_targets(force=args.force)
    else:
        ensure_reviewed_targets()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
