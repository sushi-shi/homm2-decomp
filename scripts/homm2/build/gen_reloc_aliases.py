#!/usr/bin/env python3
"""Generate exact-address owner aliases from per-function relocation multisets."""

import argparse
import csv
import json
from collections import Counter, defaultdict
from pathlib import Path

from homm2.build.assert_relocs import load_symbols, parse_obj, resolve
from homm2.build.canonicalize_relocs import load_retail_symbols
from homm2.build.reloc_alias_metadata import RelocAlias, write_aliases


def choose_aliases(proofs):
    """Retain only groups with one proven owner/addend spelling.

    Multiple spellings for one function/address cannot be assigned to retail
    sites after code movement without inventing correspondence. They are
    returned as diagnostics instead.
    """
    aliases = []
    ambiguous = []
    for (function_rva, target_rva), choices in sorted(proofs.items()):
        if not choices:
            continue
        if len(choices) != 1:
            ambiguous.append({
                "function_rva": function_rva,
                "target_rva": target_rva,
                "spellings": [
                    {"owner": owner, "addend": addend, "count": count}
                    for (owner, addend), count in sorted(choices.items())
                ],
            })
            continue
        (owner, addend), count = next(iter(choices.items()))
        if addend != 0:
            aliases.append(RelocAlias(
                function_rva, target_rva, owner, addend, count,
                "candidate-function-reloc-multiset"))
    return aliases, ambiguous


def load_manifest_symbols(path):
    symbols = {}
    by_name = defaultdict(set)
    with open(path, newline="", encoding="utf-8") as stream:
        rows = csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t")
        for row in rows:
            object_name = row["object"].replace("\\", "/")
            unit = object_name[:-2] if object_name.endswith(".c") else object_name
            key = (unit, row["name"])
            rva = int(row["rva"], 0)
            previous = symbols.setdefault(key, rva)
            if previous != rva:
                raise ValueError("conflicting TU-local data identity: %s %s" % key)
            by_name[row["name"]].add(rva)
    unique = {name: next(iter(rvas)) for name, rvas in by_name.items()
              if len(rvas) == 1}
    return symbols, unique


def collect_proofs(base_dir, target_dir, report_path, symbols_path, data_manifest_path):
    public_data, function_rvas = load_retail_symbols(symbols_path)
    symbols, data, _duplicates = load_symbols()
    manifest_symbols, unique_manifest_symbols = load_manifest_symbols(
        data_manifest_path)
    report = json.loads(Path(report_path).read_text(encoding="utf-8"))
    functions = {
        unit["name"]: {function["name"] for function in unit.get("functions", [])}
        for unit in report["units"]
    }
    proofs = defaultdict(Counter)
    coverage = Counter()
    diagnostics = []
    contexts = {}
    for unit, names in sorted(functions.items()):
        base_path = Path(base_dir) / (unit + ".obj")
        target_path = Path(target_dir) / (unit + ".c.obj")
        if not names or not base_path.exists() or not target_path.exists():
            continue
        base_functions = parse_obj(str(base_path))
        target_functions = parse_obj(str(target_path))
        coverage["functions"] += len(names)
        for name in sorted(names):
            function_rva = function_rvas.get((unit, name))
            if (name not in base_functions or name not in target_functions or
                    function_rva is None):
                continue
            coverage["paired_functions"] += 1
            target_rvas = Counter()
            for target in target_functions[name]:
                if target[0] != "DIR32":
                    continue
                target_rva = resolve(symbols, data, *target)
                if target_rva is None:
                    owner_rva = manifest_symbols.get((unit, target[1]))
                    if owner_rva is None:
                        owner_rva = unique_manifest_symbols.get(target[1])
                    if owner_rva is not None:
                        target_rva = (owner_rva + target[2]) & 0xFFFFFFFF
                if target_rva is None:
                    coverage["unresolved_target_dir32"] += 1
                    continue
                target_rvas[target_rva] += 1
                coverage["resolved_target_dir32"] += 1

            matched = Counter()
            for base_type, base_symbol, base_addend in base_functions[name]:
                if base_type != "DIR32":
                    continue
                coverage["base_dir32"] += 1
                owner_rva = public_data.get(base_symbol)
                if owner_rva is None:
                    coverage["non_public_base_dir32"] += 1
                    continue
                target_rva = (owner_rva + base_addend) & 0xFFFFFFFF
                if matched[target_rva] >= target_rvas[target_rva]:
                    coverage["unpaired_public_base_dir32"] += 1
                    continue
                matched[target_rva] += 1
                proofs[(function_rva, target_rva)][(base_symbol, base_addend)] += 1
                coverage["address_paired_public_dir32"] += 1
            for target_rva, target_count in target_rvas.items():
                key = (function_rva, target_rva)
                contexts[key] = (unit, name)
                proof_count = sum(proofs.get(key, {}).values())
                if proof_count == target_count:
                    coverage["complete_alias_groups"] += 1
                    continue
                if proof_count:
                    del proofs[key]
                    coverage["partial_alias_groups"] += 1
                    diagnostics.append({
                        "kind": "partial",
                        "unit": unit,
                        "function": name,
                        "function_rva": function_rva,
                        "target_rva": target_rva,
                        "target_count": target_count,
                        "proved_count": proof_count,
                    })
    return proofs, coverage, diagnostics, contexts


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-dir", default="build/objdiff/base")
    parser.add_argument("--target-dir", default="build/delink")
    parser.add_argument("--report", default="build/objdiff/report.json")
    parser.add_argument("--symbols", default="build/gen/symbol_names.csv")
    parser.add_argument("--data-manifest", default="build/gen/delink_data_manifest.tsv")
    parser.add_argument("--out", default="build/gen/reloc_alias_proposals.tsv")
    parser.add_argument(
        "--diagnostics", default="build/gen/reloc_alias_diagnostics.json")
    args = parser.parse_args(argv)
    proofs, coverage, diagnostics, contexts = collect_proofs(
        args.base_dir, args.target_dir, args.report, args.symbols,
        args.data_manifest)
    aliases, ambiguous = choose_aliases(proofs)
    for group in ambiguous:
        unit, function = contexts[(group["function_rva"], group["target_rva"])]
        group.update({"kind": "ambiguous", "unit": unit, "function": function})
        diagnostics.append(group)
    write_aliases(args.out, aliases)
    diagnostics.sort(key=lambda row: (
        row["unit"], row["function_rva"], row["target_rva"], row["kind"]))
    Path(args.diagnostics).parent.mkdir(parents=True, exist_ok=True)
    Path(args.diagnostics).write_text(json.dumps({
        "schema": 1,
        "partial_groups": sum(row["kind"] == "partial" for row in diagnostics),
        "ambiguous_groups": sum(row["kind"] == "ambiguous" for row in diagnostics),
        "groups": diagnostics,
    }, indent=2) + "\n", encoding="utf-8")
    print("reloc aliases: %d spelling rows from %d per-function address proofs" % (
        len(aliases), coverage["address_paired_public_dir32"]))
    print("coverage: functions=%d paired_functions=%d base_dir32=%d "
          "resolved_target_dir32=%d unresolved_target_dir32=%d "
          "non_public_base_dir32=%d unpaired_public_base_dir32=%d "
          "complete_alias_groups=%d partial_alias_groups=%d" % (
              coverage["functions"], coverage["paired_functions"],
              coverage["base_dir32"], coverage["resolved_target_dir32"],
              coverage["unresolved_target_dir32"],
              coverage["non_public_base_dir32"],
              coverage["unpaired_public_base_dir32"],
              coverage["complete_alias_groups"],
              coverage["partial_alias_groups"]))
    print("reloc alias diagnostics: %d partial, %d ambiguous -> %s" % (
        sum(row["kind"] == "partial" for row in diagnostics),
        sum(row["kind"] == "ambiguous" for row in diagnostics),
        args.diagnostics))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
