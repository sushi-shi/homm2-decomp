"""Generate bootstrap manifests and reproduce canonical targets from source data.

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
from collections import Counter
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
    manifest_bytes as contribution_manifest_bytes,
)
from homm2.build.candidate_data_manifest import derive_allocations
from homm2.build.candidate_data_manifest import (
    DIAGNOSTICS_OUTPUT as CANDIDATE_DIAGNOSTICS,
    diagnostics_bytes as candidate_diagnostics_bytes,
)
from homm2.build.audit_text_coverage import main as audit_text_coverage
from homm2.build.whole_image_coverage import (
    CoverageDiagnostic,
    build_coverage,
    diagnostics_bytes as coverage_diagnostics_bytes,
    manifest_bytes as coverage_manifest_bytes,
)
from homm2.build.data_manifest_adapter import (
    BREAKPOINTS as DATA_BREAKPOINTS,
    COMBINED_MANIFEST as CANONICAL_MANIFEST,
    SECTION_MANIFEST as CANONICAL_SECTION_MANIFEST,
    SOURCE_MANIFEST as DATA_SOURCE_MANIFEST,
    SUPPLEMENTAL as CANONICAL_SUPPLEMENTAL,
    SYMBOL_HEADER as DATA_SYMBOL_HEADER,
    build_manifests as build_data_manifests,
    validate_symbol_rows,
)
from homm2.build.data_topology_census import _fallback_kind


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SYMBOLS = REPO / "build/gen/symbol_names.csv"
LEDGER = REPO / "config/required_initialized_storage.tsv"
EXE = REPO / "build/orig/HEROES2W.EXE"
PDB = REPO / "build/pdb/HEROES2W.pdb"
RELOC_ALIASES = REPO / "config/delink_reloc_aliases.tsv"
MANIFEST = REPO / "build/gen/reviewed_delink_data.tsv"
TARGET = REPO / "build/delink"
STAMP = TARGET / ".reviewed-data-stamp.json"
CANONICAL_CONTRIBUTION_MANIFEST = CONTRIBUTION_MANIFEST
COVERAGE_PROPOSAL = REPO / "build/gen/retail_coverage.tsv"
COVERAGE_DIAGNOSTICS = REPO / "build/gen/retail_coverage_diagnostics.json"
TEXT_COVERAGE_PROPOSAL = REPO / "build/gen/retail_text_coverage.tsv"
CANONICAL_COVERAGE = COVERAGE_PROPOSAL
CANONICAL_COVERAGE_DIAGNOSTICS = COVERAGE_DIAGNOSTICS
REVIEW_QUEUE = REPO / "build/gen/data_topology_review_queue.tsv"
REVIEW_QUEUE_HEADER = (
    "row_kind", "name", "object", "rva", "size", "storage", "alignment",
    "section_offset", "scope", "evidence", "proof_count",
    "group_blockers", "group_contradictions", "candidate_provenance",
)


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
        "schema": 7,
        "mode": "canonical",
        "source_data_manifest_sha256": _digest(DATA_SOURCE_MANIFEST),
        "supplemental_manifest_sha256": _digest(CANONICAL_SUPPLEMENTAL),
        "data_manifest_sha256": _digest(CANONICAL_MANIFEST),
        "data_section_manifest_sha256": _digest(CANONICAL_SECTION_MANIFEST),
        "data_breakpoints_sha256": _digest(DATA_BREAKPOINTS),
        "contribution_manifest_sha256": _digest(CANONICAL_CONTRIBUTION_MANIFEST),
        "coverage_manifest_sha256": _digest(CANONICAL_COVERAGE),
        "coverage_diagnostics_sha256": _digest(CANONICAL_COVERAGE_DIAGNOSTICS),
        "exe_sha256": _digest(EXE),
        "pdb_sha256": _digest(PDB),
        "reloc_aliases_sha256": _digest(RELOC_ALIASES),
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


def _read_symbol_manifest(path, label):
    path = Path(path)
    if not path.is_file():
        raise RuntimeError("%s manifest is missing: %s" % (label, path))
    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t")
        if tuple(reader.fieldnames or ()) != DATA_SYMBOL_HEADER:
            raise RuntimeError("%s manifest has an invalid header: %s" % (label, path))
        rows = list(reader)
    try:
        validate_symbol_rows(rows, label)
    except (KeyError, TypeError, ValueError) as error:
        raise RuntimeError("%s manifest is invalid: %s" % (label, error)) from error
    return rows


def _canonical_union_rows(source_manifest=None, supplemental=None,
                          combined_manifest=None):
    source_manifest = source_manifest or DATA_SOURCE_MANIFEST
    supplemental = supplemental or CANONICAL_SUPPLEMENTAL
    combined_manifest = combined_manifest or CANONICAL_MANIFEST
    source_rows = _read_symbol_manifest(source_manifest, "source DATA")
    supplemental_rows = _read_symbol_manifest(supplemental, "reviewed supplemental")
    combined_rows = _read_symbol_manifest(combined_manifest, "canonical combined")

    def row_tuple(row):
        return tuple(row[key] for key in DATA_SYMBOL_HEADER)

    expected = Counter(row_tuple(row) for row in [*source_rows, *supplemental_rows])
    actual = Counter(row_tuple(row) for row in combined_rows)
    if actual != expected:
        raise RuntimeError(
            "canonical data manifest is not the exact source DATA plus reviewed "
            "supplemental union; run `homm2 data-topology assemble`")
    return combined_rows


def _review_evidence(provenance):
    return {
        "candidate-coff-section-translation": "exact-section-translation",
        "candidate-coff-reloc-bijection": "aligned-relocation-bijection",
    }.get(provenance, provenance)


def _synthetic_candidate_identity(name):
    lower = name.lower()
    return (_fallback_kind(lower) is not None or lower.startswith((
        "unresolved_", "fallback_", "__homm2_data_alias$")))


def review_queue_bytes(allocations, diagnostics=(), source_manifest=None,
                       supplemental=None, combined_manifest=None):
    """Return non-canonical real candidate rows as a review-only queue."""
    canonical = _canonical_union_rows(
        source_manifest, supplemental, combined_manifest)
    canonical_identities = {(row["object"], row["name"]) for row in canonical}
    proposed = [(allocation, (), ()) for allocation in allocations]
    proposed.extend(
        (allocation, diagnostic.causes, diagnostic.details)
        for diagnostic in diagnostics
        for allocation in diagnostic.proposed_allocations)
    queue = []
    queued_identities = set()
    already_canonical = 0
    synthetic_filtered = 0
    for allocation, blockers, contradictions in proposed:
        identity = (allocation.object_name, allocation.name)
        if identity in canonical_identities:
            already_canonical += 1
            continue
        if _synthetic_candidate_identity(allocation.name):
            synthetic_filtered += 1
            continue
        if identity in queued_identities:
            continue
        queued_identities.add(identity)
        queue.append({
            "row_kind": "allocation-symbol",
            "name": allocation.name,
            "object": allocation.object_name,
            "rva": "0x%x" % allocation.rva,
            "size": "0x%x" % allocation.size,
            "storage": allocation.storage,
            "alignment": "0x%x" % allocation.alignment,
            "section_offset": "0x%x" % allocation.section_offset,
            "scope": allocation.scope,
            "evidence": _review_evidence(allocation.provenance),
            "proof_count": str(allocation.proof_count),
            "group_blockers": json.dumps(list(blockers), separators=(",", ":")),
            "group_contradictions": json.dumps(
                list(contradictions), separators=(",", ":")),
            "candidate_provenance": allocation.provenance,
        })
    lines = [
        "# Review queue only; never a delinker input and never auto-promoted.",
        "\t".join(REVIEW_QUEUE_HEADER),
    ]
    for row in sorted(queue, key=lambda value: (
            value["object"], value["storage"], int(value["section_offset"], 0),
            value["name"])):
        lines.append("\t".join(row[key] for key in REVIEW_QUEUE_HEADER))
    stats = {
        "candidate_rows": len(proposed),
        "canonical_rows": len(canonical),
        "already_canonical": already_canonical,
        "review_queue_rows": len(queue),
        "synthetic_filtered": synthetic_filtered,
    }
    return ("\n".join(lines) + "\n").encode("utf-8"), stats


def _build_reviewed_canonical_manifests(strict):
    """Assemble canonical rows only from source DATA and the reviewed supplement."""
    supplemental_rows = _read_symbol_manifest(
        CANONICAL_SUPPLEMENTAL, "reviewed supplemental")
    synthetic = [row["name"] for row in supplemental_rows
                 if _synthetic_candidate_identity(row["name"])]
    if synthetic:
        raise RuntimeError(
            "reviewed supplemental contains forbidden synthetic identities: %s" %
            ", ".join(sorted(synthetic)))
    return build_data_manifests(
        supplemental=CANONICAL_SUPPLEMENTAL, migrate_from=None, strict=strict)


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
            DATA_SOURCE_MANIFEST, CANONICAL_SUPPLEMENTAL, CANONICAL_MANIFEST,
            CANONICAL_SECTION_MANIFEST, DATA_BREAKPOINTS,
            CANONICAL_CONTRIBUTION_MANIFEST,
            CANONICAL_COVERAGE, CANONICAL_COVERAGE_DIAGNOSTICS,
            EXE, PDB)
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
    """Write review-only candidate rows and diagnostics under build/gen."""
    canonical = _canonical_union_rows()
    allocations, stats, diagnostics = derive_allocations(reviewed_rows=canonical)
    queue, queue_stats = review_queue_bytes(allocations, diagnostics)
    _atomic_write(REVIEW_QUEUE, queue)
    _atomic_write(CANDIDATE_DIAGNOSTICS,
                  candidate_diagnostics_bytes(stats, diagnostics))
    return stats, diagnostics, queue_stats


def _build_coverage_proposal(allocations):
    closed_groups = {(row.unit, row.storage) for row in allocations}
    rows, padding, diagnostics = build_coverage(
        allocations, EXE, SYMBOLS, REPO / "config/units.toml",
        closed_groups=closed_groups,
        contributions_path=CANONICAL_CONTRIBUTION_MANIFEST)
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


def promote_canonical_topology(require_all=False):
    """Refresh generated evidence; never copy derived data into config."""
    if not require_all:
        _build_reviewed_canonical_manifests(strict=False)
    canonical = _canonical_union_rows()
    allocations, stats, group_diagnostics = derive_allocations(reviewed_rows=canonical)
    _atomic_write(CANDIDATE_DIAGNOSTICS,
                  candidate_diagnostics_bytes(stats, group_diagnostics))
    _coverage, _padding, coverage_diagnostics = _build_coverage_proposal(allocations)
    blockers = [*group_diagnostics, *coverage_diagnostics]
    if require_all and blockers:
        return stats, blockers
    if require_all:
        _build_reviewed_canonical_manifests(strict=True)
    return stats, blockers


def regenerate_canonical_targets(delinker=None):
    """Regenerate canonical inputs and atomically replace targets."""
    _build_reviewed_canonical_manifests(strict=True)
    canonical = _canonical_union_rows()
    allocations, _stats, group_diagnostics = derive_allocations(reviewed_rows=canonical)
    _coverage, _padding, coverage_diagnostics = _build_coverage_proposal(allocations)
    if group_diagnostics or coverage_diagnostics:
        raise RuntimeError(
            "canonical regeneration requires closed data and whole-image coverage; "
            "see %s and %s" % (CANDIDATE_DIAGNOSTICS, COVERAGE_DIAGNOSTICS))
    missing = [path for path in (
        DATA_SOURCE_MANIFEST, CANONICAL_SUPPLEMENTAL, CANONICAL_MANIFEST,
        CANONICAL_SECTION_MANIFEST, DATA_BREAKPOINTS,
        CANONICAL_CONTRIBUTION_MANIFEST,
        CANONICAL_COVERAGE, CANONICAL_COVERAGE_DIAGNOSTICS,
        EXE, PDB, RELOC_ALIASES)
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
            "--data-section-manifest", str(CANONICAL_SECTION_MANIFEST),
            "--contribution-manifest", str(CANONICAL_CONTRIBUTION_MANIFEST),
            "--reloc-alias-manifest", str(RELOC_ALIASES),
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
        stats, diagnostics, queue = propose_candidate_topology()
        print("candidate data: %d/%d individually evidenced; %d/%d definitions in "
              "%d closed groups; %d open groups; %d review rows "
              "(%d already canonical, %d synthetic filtered)" % (
            stats.evidenced_definitions, stats.candidate_definitions,
            stats.mapped_definitions, stats.candidate_definitions,
            stats.closed_groups, stats.open_groups, queue["review_queue_rows"],
            queue["already_canonical"], queue["synthetic_filtered"]))
        return 0
    elif args.promote or args.finalize:
        stats, diagnostics = promote_canonical_topology(require_all=args.finalize)
        if args.finalize and diagnostics:
            print("candidate data: refusing final closure with %d blocking diagnostics; "
                  "see %s and %s" % (
                      len(diagnostics), CANDIDATE_DIAGNOSTICS, COVERAGE_DIAGNOSTICS))
            return 1
        print("candidate data: promoted snapshot with %d/%d individually evidenced; "
              "%d definitions in %d closed groups; %d open diagnostics" % (
                  stats.evidenced_definitions, stats.candidate_definitions,
                  stats.mapped_definitions, stats.closed_groups, len(diagnostics)))
    else:
        regenerate_canonical_targets()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
