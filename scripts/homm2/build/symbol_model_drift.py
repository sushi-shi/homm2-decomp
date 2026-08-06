"""Fast, warning-only drift check between candidate COFF and fixed symbol models."""

import argparse
import csv
import os
import re
import tomllib
from collections import Counter, defaultdict
from pathlib import Path

from homm2.build.canonicalize_data_symbols import CoffObject, VOLATILE_E_FUNCTION
from homm2.build.data_manifest_adapter import (
    _compgen_candidate_kind,
    candidate_topology,
)


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
UNMODELED_TARGET = re.compile(
    r"^(?:_?\$E[0-9]+|\$SG[0-9]+|\$T[0-9]+|"
    r"const_|string_|data_|bss_|unresolved_|fallback_|"
    r"__homm2_data_alias\$|\[section-[0-9]+\])")


def _csv_rows(path, delimiter=","):
    with Path(path).open(newline="", encoding="utf-8") as stream:
        return list(csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter=delimiter))


def _counter_drift(label, actual, expected):
    missing = expected - actual
    extra = actual - expected
    rows = []
    if missing:
        rows.append(f"{label}: {sum(missing.values())} modeled identities not emitted")
    if extra:
        sample = ", ".join(name for name, _count in extra.most_common(4))
        rows.append(
            f"{label}: {sum(extra.values())} emitted identities are not modeled"
            + (f" ({sample})" if sample else ""))
    return rows


def audit_symbol_model(repo=REPO):
    """Return cheap drift diagnostics; never perform retail binding or delinking."""
    repo = Path(repo)
    units_path = repo / "config/units.toml"
    base_root = repo / "build/objdiff/base"
    target_root = repo / "build/delink"
    data_path = repo / "build/gen/delink_data_from_source.tsv"
    symbols_path = repo / "build/gen/symbol_names.csv"
    private_path = repo / "build/gen/source_private_functions.csv"
    compgen_path = repo / "build/gen/compiler_generated_functions.csv"
    required = (units_path, data_path, symbols_path, private_path, compgen_path)
    absent = [str(path.relative_to(repo)) for path in required if not path.is_file()]
    if absent:
        return ["missing model inputs: " + ", ".join(absent)]

    units = [row["unit"] for row in tomllib.loads(
        units_path.read_text()).get("unit", [])]
    data_rows = _csv_rows(data_path, "\t")
    data_by_unit = defaultdict(list)
    for row in data_rows:
        unit = row["object"].replace("\\", "/")
        if not unit.endswith(".c"):
            return [f"invalid modeled object path: {row['object']}"]
        data_by_unit[unit[:-2]].append(row)

    symbol_rows = _csv_rows(symbols_path)
    global_functions = {
        row["name"] for row in symbol_rows if row.get("kind") == "func"}
    private_functions = defaultdict(Counter)
    for row in _csv_rows(private_path):
        private_functions[row["unit"]][row["name"]] += 1
    expected_compgen_functions = Counter(
        row["unit"] for row in _csv_rows(compgen_path))

    diagnostics = []
    actual_compgen_functions = Counter()
    for unit in units:
        object_path = base_root / f"{unit}.obj"
        if not object_path.is_file():
            diagnostics.append(f"{unit}: candidate object is missing")
            continue
        definitions, _sections = candidate_topology(object_path, unit)
        candidate_compgen = [
            row for row in definitions if _compgen_candidate_kind(row) is not None]
        modeled_compgen = [
            row for row in data_by_unit[unit]
            if row["provenance"].startswith("source-DATA_COMPGEN:")]
        if len(candidate_compgen) != len(modeled_compgen):
            diagnostics.append(
                f"{unit}: compiler-data count changed "
                f"({len(candidate_compgen)} emitted, {len(modeled_compgen)} modeled)")

        candidate_named = Counter(
            row.symbol for row in definitions
            if _compgen_candidate_kind(row) is None)
        modeled_named = Counter(
            row["name"] for row in data_by_unit[unit]
            if not row["provenance"].startswith("source-DATA_COMPGEN:"))
        diagnostics.extend(_counter_drift(
            f"{unit} data", candidate_named, modeled_named))

        coff = CoffObject(object_path.read_bytes())
        candidate_private = Counter()
        for symbol in coff.symbols.values():
            if symbol.section <= 0 or symbol.typ != 0x20:
                continue
            if VOLATILE_E_FUNCTION.match(symbol.name):
                actual_compgen_functions[unit] += 1
            elif symbol.storage_class == 2:
                if symbol.name not in global_functions:
                    diagnostics.append(
                        f"{unit}: emitted external function is not modeled "
                        f"({symbol.name})")
            elif symbol.storage_class == 3:
                candidate_private[symbol.name] += 1
        diagnostics.extend(_counter_drift(
            f"{unit} private functions", candidate_private,
            private_functions[unit]))

    for unit in sorted(set(units) | set(expected_compgen_functions)):
        if actual_compgen_functions[unit] != expected_compgen_functions[unit]:
            diagnostics.append(
                f"{unit}: compiler-function count changed "
                f"({actual_compgen_functions[unit]} emitted, "
                f"{expected_compgen_functions[unit]} modeled)")

    if not target_root.is_dir():
        diagnostics.append("fixed delink target is missing")
    else:
        unstable = []
        for path in sorted(target_root.rglob("*.obj")):
            coff = CoffObject(path.read_bytes())
            unstable.extend(
                f"{path.relative_to(target_root)}:{symbol.name}"
                for symbol in coff.symbols.values()
                if symbol.section > 0 and UNMODELED_TARGET.match(symbol.name))
        if unstable:
            diagnostics.append(
                f"fixed target retains {len(unstable)} unmodeled/fallback identities "
                f"({', '.join(unstable[:4])})")
    return diagnostics


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.parse_args(argv)
    try:
        diagnostics = audit_symbol_model()
    except (KeyError, OSError, ValueError) as error:
        diagnostics = [f"check could not complete: {error}"]
    if diagnostics:
        print("[symbol-model] WARN: fixed delink model may be stale; "
              "run `homm2 redelink` when ready")
        for row in diagnostics[:12]:
            print(f"  {row}")
        if len(diagnostics) > 12:
            print(f"  ... {len(diagnostics) - 12} more")
    else:
        print("[symbol-model] current: no unmodeled or fallback identities")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
