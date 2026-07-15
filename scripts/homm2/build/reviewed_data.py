"""Generate bootstrap data manifests and explicitly promote canonical targets.

NB09 public-data records prove names, addresses, and compiland ownership, but do not
carry allocation lengths.  Exact extents in this module always come from the reviewed
initialized-storage ledger; the provisional next-public gaps in symbol_names.csv are
never used as sizes.  Normal build and status commands consume an immutable target;
they never derive candidate topology or regenerate target objects.
"""
import argparse
import csv
import hashlib
import json
import os
import shutil
import subprocess
import tempfile
from collections import defaultdict
from pathlib import Path

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
from homm2.build.contribution_manifest import (
    OUTPUT as CONTRIBUTION_MANIFEST,
    contribution_rows,
    manifest_bytes as contribution_manifest_bytes,
)
from homm2.build.candidate_data_manifest import _pe_layout, derive_allocations
from homm2.build.candidate_data_manifest import (
    OUTPUT as CANDIDATE_PROPOSAL,
    DIAGNOSTICS_OUTPUT as CANDIDATE_DIAGNOSTICS,
    diagnostics_bytes as candidate_diagnostics_bytes,
    manifest_bytes as candidate_manifest_bytes,
)
from homm2.build.audit_text_coverage import main as audit_text_coverage
from homm2.build.whole_image_coverage import (
    CoverageDiagnostic,
    build_coverage,
    diagnostics_bytes as coverage_diagnostics_bytes,
    manifest_bytes as coverage_manifest_bytes,
)


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SYMBOLS = REPO / "build/gen/symbol_names.csv"
LEDGER = REPO / "config/required_initialized_storage.tsv"
EXE = REPO / "build/orig/HEROES2W.EXE"
PDB = REPO / "build/pdb/HEROES2W.pdb"
MANIFEST = REPO / "build/gen/reviewed_delink_data.tsv"
TARGET = REPO / "build/delink"
STAMP = TARGET / ".reviewed-data-stamp.json"
CANONICAL_MANIFEST = REPO / "config/delink_data_topology.tsv"
CANONICAL_CONTRIBUTION_MANIFEST = REPO / "config/delink_contributions.tsv"
COVERAGE_PROPOSAL = REPO / "build/gen/retail_coverage.tsv"
COVERAGE_DIAGNOSTICS = REPO / "build/gen/retail_coverage_diagnostics.json"
TEXT_COVERAGE_PROPOSAL = REPO / "build/gen/retail_text_coverage.tsv"
CANONICAL_COVERAGE = REPO / "config/retail_coverage.tsv"
CANONICAL_COVERAGE_DIAGNOSTICS = REPO / "config/retail_coverage_diagnostics.json"
CANONICAL_UNRESOLVED = REPO / "config/delink_unresolved_data.tsv"


def _digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def reviewed_manifest_bytes(symbols=SYMBOLS, ledger=LEDGER, exe=EXE,
                            candidate_allocations=(), padding_allocations=()):
    with Path(symbols).open(newline="", encoding="latin-1") as stream:
        inventory = {row["name"]: row for row in csv.DictReader(stream)}
    retail = read_pe(exe)
    rows = {}
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
            raise RuntimeError("reviewed owner is absent from public inventory: %s" %
                               owner.symbol)
        if int(symbol["rva"], 0) != owner.rva:
            raise RuntimeError("reviewed owner public RVA mismatch: %s" % owner.symbol)
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
    candidate_allocations = tuple(candidate_allocations)
    if candidate_allocations:
        closed_groups = {(row.unit, row.storage) for row in candidate_allocations}
        rows = {name: row for name, row in rows.items()
                if (row["unit"], row["storage"]) not in closed_groups}
        candidate_rows = []
        external_names = set(rows)
        local_names = set()
        for allocation in candidate_allocations:
            if allocation.scope == "external":
                if allocation.name in external_names:
                    raise RuntimeError("candidate allocation duplicates external name: %s" %
                                       allocation.name)
                external_names.add(allocation.name)
            else:
                key = (allocation.unit, allocation.name)
                if key in local_names:
                    raise RuntimeError(
                        "candidate allocation duplicates local owner/name: %s %s" % key)
                local_names.add(key)
            candidate_rows.append({
                "unit": allocation.unit,
                "rva": allocation.rva,
                "name": allocation.name,
                "size": allocation.size,
                "storage": allocation.storage,
                "alignment": allocation.alignment,
                "section_offset": allocation.section_offset,
                "scope": allocation.scope,
                "provenance": allocation.provenance,
            })
    else:
        candidate_rows = []
    for padding in padding_allocations:
        candidate_rows.append({
            "unit": padding.unit,
            "rva": padding.rva,
            "name": padding.name,
            "size": padding.size,
            "storage": padding.storage,
            "alignment": 1,
            "scope": "local",
            "provenance": "explicit-retail-zero-padding",
        })

    lines = [
        "# Explicit reviewed allocations plus complete candidate-COFF topology groups.",
        "name\tobject\trva\tsize\tstorage\talignment\tsection_offset\tscope\tprovenance",
    ]
    for row in sorted([*rows.values(), *candidate_rows], key=lambda value: (
            value["unit"], value["storage"],
            value.get("section_offset", value["rva"]), value["rva"])):
        object_name = row["unit"].replace("/", "\\") + ".c"
        alignment = row.get("alignment", 4)
        section_offset = row.get("section_offset")
        lines.append("%s\t%s\t0x%x\t0x%x\t%s\t0x%x\t%s\t%s\t%s" % (
            row["name"], object_name, row["rva"], row["size"],
            row["storage"], alignment,
            ("0x%x" % section_offset) if section_offset is not None else "-",
            row.get("scope", "external"), row["provenance"]))
    return ("\n".join(lines) + "\n").encode("utf-8")


def _identity_inputs(delinker, contribution_manifest, manifest):
    definitions = load_definition_rvas()
    owner_definitions = "\n".join(
        "%s=0x%x" % (name, definitions.get(name, -1))
        for name in sorted(load_explicit_extents()))
    return {
        "schema": 4,
        "ledger_sha256": _digest(LEDGER),
        "owner_extents_sha256": _digest(REPO / OWNER_EXTENTS),
        "owner_definitions_sha256": hashlib.sha256(
            owner_definitions.encode("utf-8")).hexdigest(),
        "symbols_sha256": _digest(SYMBOLS),
        "exe_sha256": _digest(EXE),
        "pdb_sha256": _digest(PDB),
        "delinker_sha256": _digest(delinker),
        "adapter_sha256": _digest(__file__),
        "contribution_manifest_sha256": hashlib.sha256(
            contribution_manifest).hexdigest(),
        "manifest_sha256": hashlib.sha256(manifest).hexdigest(),
    }


def _identity(manifest, contribution_manifest, delinker):
    return _identity_inputs(delinker, contribution_manifest, manifest)


def _canonical_identity_inputs(delinker):
    return {
        "schema": 5,
        "mode": "canonical",
        "data_manifest_sha256": _digest(CANONICAL_MANIFEST),
        "contribution_manifest_sha256": _digest(CANONICAL_CONTRIBUTION_MANIFEST),
        "coverage_manifest_sha256": _digest(CANONICAL_COVERAGE),
        "coverage_diagnostics_sha256": _digest(CANONICAL_COVERAGE_DIAGNOSTICS),
        "unresolved_manifest_sha256": _digest(CANONICAL_UNRESOLVED),
        "exe_sha256": _digest(EXE),
        "pdb_sha256": _digest(PDB),
        "delinker_sha256": _digest(delinker),
    }


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
    """Validate the fixed target without deriving data or changing target objects."""
    if not TARGET.is_dir():
        raise RuntimeError("delinked target is missing; run `homm2 init`")
    try:
        current = json.loads(STAMP.read_text())
    except (FileNotFoundError, json.JSONDecodeError):
        raise RuntimeError("delinked target has no valid provenance stamp; run `homm2 init`")
    if current.get("mode") == "canonical":
        missing = [path for path in (
            CANONICAL_MANIFEST, CANONICAL_CONTRIBUTION_MANIFEST,
            CANONICAL_COVERAGE, CANONICAL_COVERAGE_DIAGNOSTICS,
            CANONICAL_UNRESOLVED, EXE, PDB)
            if not path.is_file()]
        delinker = Path(delinker or shutil.which("vostok-delinker") or "")
        if missing or not delinker.is_file():
            raise RuntimeError(
                "canonical target inputs are missing; restore them and run "
                "`homm2 data-topology regenerate`")
        expected = _canonical_identity_inputs(delinker)
        if refresh_required(current, expected):
            raise RuntimeError(
                "canonical target inputs changed; run `homm2 data-topology regenerate` "
                "explicitly")
    _refresh_objdiff_targets()
    return False


def propose_candidate_topology():
    """Write a disposable partial proposal and diagnostics under build/gen."""
    payload, stats, diagnostics = candidate_manifest_bytes()
    _atomic_write(CANDIDATE_PROPOSAL, payload)
    _atomic_write(CANDIDATE_DIAGNOSTICS,
                  candidate_diagnostics_bytes(stats, diagnostics))
    allocations, _stats, _diagnostics = derive_allocations()
    _build_coverage_proposal(allocations)
    return stats, diagnostics


def _build_coverage_proposal(allocations):
    closed_groups = {(row.unit, row.storage) for row in allocations}
    rows, padding, diagnostics = build_coverage(
        allocations, EXE, SYMBOLS, REPO / "config/units.toml",
        closed_groups=closed_groups)
    try:
        TEXT_COVERAGE_PROPOSAL.unlink()
    except FileNotFoundError:
        pass
    text_result = audit_text_coverage([
        str(EXE), str(SYMBOLS), str(REPO / "config/delink_procedures.csv"),
        str(TEXT_COVERAGE_PROPOSAL),
    ])
    if text_result:
        diagnostics.append(CoverageDiagnostic(
            "text_coverage_gate_failed", "PE", "text", 0, 0,
            "homm2.build.audit_text_coverage reported evidence failures"))
    payload = coverage_manifest_bytes(rows)
    if TEXT_COVERAGE_PROPOSAL.is_file():
        text_lines = TEXT_COVERAGE_PROPOSAL.read_bytes().splitlines()
        payload = payload.rstrip(b"\n") + b"\n" + b"\n".join(text_lines[1:]) + b"\n"
    _atomic_write(COVERAGE_PROPOSAL, payload)
    _atomic_write(COVERAGE_DIAGNOSTICS, coverage_diagnostics_bytes(diagnostics))
    return payload, padding, diagnostics


def unresolved_manifest_bytes(group_diagnostics, promoted_manifest):
    contributions = defaultdict(list)
    for row in contribution_rows():
        if row["storage"] == "text":
            continue
        unit = row["object"].replace("\\", "/").removesuffix(".c")
        contributions[(unit, row["storage"])].append((
            row["rva"], row["size"], row["segment"], row["section"]))
    records = []
    for diagnostic in sorted(group_diagnostics, key=lambda row: (row.unit, row.storage)):
        ranges = contributions.get((diagnostic.unit, diagnostic.storage), [])
        provenance = "retail-nb09-sstModule-open-group"
        if not ranges:
            segment = 2 if diagnostic.storage == "rdata" else 3
            section = ".rdata" if diagnostic.storage == "rdata" else ".data"
            ranges = [(rva, size, segment, section)
                      for rva, size, _name in diagnostic.evidence_ranges]
            provenance = "candidate-reloc-evidence-without-sstModule"
        for rva, size, segment, section in ranges:
            records.append({
                "storage": diagnostic.storage, "rva": rva, "size": size,
                "segment": segment, "section": section,
                "identity": "%s:%s:%s" % (
                    diagnostic.unit, provenance, ",".join(diagnostic.causes)),
            })
    promoted = []
    lines = [line for line in promoted_manifest.decode("utf-8").splitlines()
             if line and not line.startswith("#")]
    for row in csv.DictReader(lines, delimiter="\t"):
        promoted.append((int(row["rva"], 0), int(row["size"], 0)))
    retail = read_pe(EXE)
    image_base, highlow, read_u32, _read_bytes = _pe_layout(EXE)
    for site in highlow:
        target = (read_u32(site) - image_base) & 0xFFFFFFFF
        if any(rva <= target < rva + size for rva, size in promoted):
            continue
        storage_class = classify_pe_storage(retail, target)["class"]
        storage = {
            "rdata": "rdata",
            "data-initialized": "data",
            "data-loader-zero-tail": "bss",
        }.get(storage_class)
        if storage is None:
            continue
        records.append({
            "storage": storage, "rva": target, "size": 1,
            "segment": 2 if storage == "rdata" else 3,
            "section": ".rdata" if storage == "rdata" else ".data",
            "identity": "HIGHLOW@0x%x:retail-unowned-target" % site,
        })
    merged = []
    for row in sorted(records, key=lambda value: (value["storage"], value["rva"])):
        if (merged and merged[-1]["storage"] == row["storage"] and
                row["rva"] <= merged[-1]["rva"] + merged[-1]["size"]):
            previous = merged[-1]
            end = max(previous["rva"] + previous["size"], row["rva"] + row["size"])
            previous["size"] = end - previous["rva"]
            previous["identities"].append(row["identity"])
        else:
            merged.append({**row, "identities": [row["identity"]]})
    lines = [
        "object\tstorage\trva\tsize\tsegment\tsection\tprovenance",
    ]
    for row in merged:
        lines.append("UNRESOLVED\\%s.c\t%s\t0x%x\t0x%x\t%s\t%s\t%s" % (
            row["storage"], row["storage"], row["rva"], row["size"],
            row["segment"], row["section"], "|".join(row["identities"])))
    return ("\n".join(lines) + "\n").encode("utf-8")


def promote_canonical_topology(require_all=False):
    """Promote a reviewed snapshot; optionally require final whole-image closure."""
    allocations, stats, group_diagnostics = derive_allocations()
    _atomic_write(CANDIDATE_DIAGNOSTICS,
                  candidate_diagnostics_bytes(stats, group_diagnostics))
    coverage, padding, coverage_diagnostics = _build_coverage_proposal(allocations)
    blockers = [*group_diagnostics, *coverage_diagnostics]
    if require_all and blockers:
        return stats, blockers
    manifest = reviewed_manifest_bytes(
        candidate_allocations=allocations,
        padding_allocations=padding if require_all else ())
    contribution_manifest = contribution_manifest_bytes()
    unresolved = unresolved_manifest_bytes(group_diagnostics, manifest)
    _atomic_write(CANONICAL_MANIFEST, manifest)
    _atomic_write(CANONICAL_CONTRIBUTION_MANIFEST, contribution_manifest)
    _atomic_write(CANONICAL_COVERAGE, coverage)
    _atomic_write(CANONICAL_COVERAGE_DIAGNOSTICS,
                  coverage_diagnostics_bytes(coverage_diagnostics))
    _atomic_write(CANONICAL_UNRESOLVED, unresolved)
    return stats, blockers


def regenerate_canonical_targets(delinker=None):
    """Atomically replace targets from committed canonical manifests only."""
    missing = [path for path in (
        CANONICAL_MANIFEST, CANONICAL_CONTRIBUTION_MANIFEST,
        CANONICAL_COVERAGE, CANONICAL_COVERAGE_DIAGNOSTICS,
        CANONICAL_UNRESOLVED, EXE, PDB)
        if not path.is_file()]
    if missing:
        raise RuntimeError("canonical target regeneration requires %s" %
                           ", ".join(str(path) for path in missing))
    delinker = Path(delinker or shutil.which("vostok-delinker") or "")
    if not delinker.is_file():
        raise RuntimeError("vostok-delinker is required for canonical regeneration")
    manifest = CANONICAL_MANIFEST.read_bytes()
    identity = _canonical_identity_inputs(delinker)
    TARGET.parent.mkdir(parents=True, exist_ok=True)
    temporary = Path(tempfile.mkdtemp(prefix="delink-canonical-", dir=TARGET.parent))
    backup = TARGET.with_name(TARGET.name + ".pre-canonical")
    try:
        subprocess.run([
            str(delinker), "--pdb-path", str(PDB), "--exe-path", str(EXE),
            "--output-path", str(temporary), "--engine-path", "c:\\proj\\",
            "--data-manifest", str(CANONICAL_MANIFEST),
            "--contribution-manifest", str(CANONICAL_CONTRIBUTION_MANIFEST),
            "--unresolved-data-manifest", str(CANONICAL_UNRESOLVED),
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


def record_current_targets(delinker=None):
    """Stamp a target tree just emitted by init without running the delinker again."""
    delinker = Path(delinker or shutil.which("vostok-delinker") or "")
    if not delinker.is_file():
        raise RuntimeError("vostok-delinker is required to record target freshness")
    manifest = reviewed_manifest_bytes()
    contribution_manifest = contribution_manifest_bytes()
    if not MANIFEST.is_file() or MANIFEST.read_bytes() != manifest:
        raise RuntimeError("generated reviewed data manifest is stale")
    if (not CONTRIBUTION_MANIFEST.is_file() or
            CONTRIBUTION_MANIFEST.read_bytes() != contribution_manifest):
        raise RuntimeError("generated contribution manifest is stale")
    _validate_owner_objects(TARGET, manifest)
    identity = _identity(manifest, contribution_manifest, delinker)
    identity["mode"] = "bootstrap"
    STAMP.write_text(json.dumps(identity, indent=2) + "\n")


def main(argv=None):
    parser = argparse.ArgumentParser()
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--write-manifest", action="store_true")
    action.add_argument("--ensure", action="store_true")
    action.add_argument("--record-current", action="store_true")
    action.add_argument("--propose", action="store_true")
    action.add_argument("--promote", action="store_true")
    action.add_argument("--finalize", action="store_true")
    action.add_argument("--regenerate", action="store_true")
    args = parser.parse_args(argv)
    if args.write_manifest:
        MANIFEST.parent.mkdir(parents=True, exist_ok=True)
        MANIFEST.write_bytes(reviewed_manifest_bytes())
        CONTRIBUTION_MANIFEST.write_bytes(contribution_manifest_bytes())
    elif args.ensure:
        ensure_reviewed_targets()
    elif args.record_current:
        record_current_targets()
    elif args.propose:
        stats, diagnostics = propose_candidate_topology()
        print("candidate data: %d/%d definitions in %d closed groups; %d open groups" % (
            stats.mapped_definitions, stats.candidate_definitions,
            stats.closed_groups, stats.open_groups))
        return 0
    elif args.promote or args.finalize:
        stats, diagnostics = promote_canonical_topology(require_all=args.finalize)
        if args.finalize and diagnostics:
            print("candidate data: refusing final closure with %d blocking diagnostics; "
                  "see %s and %s" % (
                      len(diagnostics), CANDIDATE_DIAGNOSTICS, COVERAGE_DIAGNOSTICS))
            return 1
        print("candidate data: promoted snapshot with %d definitions in %d closed groups; "
              "%d open diagnostics" % (
                  stats.mapped_definitions, stats.closed_groups, len(diagnostics)))
    else:
        regenerate_canonical_targets()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
