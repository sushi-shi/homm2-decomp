#!/usr/bin/env python3
"""Generate exact-address PDB owner aliases from paired exact relocations."""

import argparse
import struct
from collections import Counter, defaultdict
from pathlib import Path

from homm2.build.assert_relocs import IMAGE_BASE, _pe_read, parse_obj
from homm2.build.canonicalize_relocs import (
    Coverage, authorize_owner_alias, exact_functions, load_retail_symbols,
    record_site_coverage,
)
from homm2.build.assert_relocs import load_symbols
from homm2.build.reloc_alias_metadata import RelocAlias, write_aliases


def choose_aliases(proofs):
    """Choose one deterministic global owner spelling per exact target RVA.

    A PDB data-symbol address can carry only one name. Prefer the spelling with
    the most exact site proofs, then break ties by owner and unsigned addend.
    Zero-addend spellings need no alias because the public owner is already at
    the exact target address.
    """
    aliases = {}
    for target_rva, choices in sorted(proofs.items()):
        ranked = sorted(
            choices.items(), key=lambda item: (-item[1], item[0][0], item[0][1]))
        if not ranked:
            continue
        (owner, addend), count = ranked[0]
        if addend == 0:
            continue
        aliases[target_rva] = RelocAlias(
            target_rva, owner, addend, count, len(ranked) - 1)
    return aliases


def collect_proofs(base_dir, target_dir, report_path, symbols_path):
    public_data, function_rvas = load_retail_symbols(symbols_path)
    symbols, data, duplicates = load_symbols()
    exact = exact_functions(report_path)
    proofs = defaultdict(Counter)
    coverage = Coverage()
    authorized_sites = 0
    for unit, names in sorted(exact.items()):
        base_path = Path(base_dir) / (unit + ".obj")
        target_path = Path(target_dir) / (unit + ".c.obj")
        if not names or not base_path.exists() or not target_path.exists():
            continue
        base_sites = parse_obj(str(base_path), with_sites=True)
        target_sites = parse_obj(str(target_path), with_sites=True)
        coverage.exact_functions += len(names)
        for name in sorted(names):
            function_rva = function_rvas.get((unit, name))
            if (name not in base_sites or name not in target_sites or
                    function_rva is None):
                continue
            coverage.paired_functions += 1
            coverage.base_sites += len(base_sites[name])
            target_by_site = {
                relocation[0]: relocation[1:]
                for relocation in target_sites[name]
            }
            for site, base_type, base_symbol, base_addend in base_sites[name]:
                base = (base_type, base_symbol, base_addend)
                target = target_by_site.get(site)
                if not record_site_coverage(
                        coverage, base, target, symbols, data, duplicates):
                    continue
                if base_type != "DIR32" or target[0] != "DIR32":
                    continue
                retail_operand = _pe_read(function_rva + site, 4)
                if retail_operand is None:
                    continue
                target_rva = (
                    struct.unpack("<I", retail_operand)[0] - IMAGE_BASE) & 0xFFFFFFFF
                owner = authorize_owner_alias(
                    public_data, base_type, base_symbol, base_addend, target_rva)
                if owner is None:
                    continue
                proofs[target_rva][(owner, base_addend)] += 1
                authorized_sites += 1
    return proofs, coverage, authorized_sites


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-dir", default="build/objdiff/base")
    parser.add_argument("--target-dir", default="build/delink")
    parser.add_argument("--report", default="build/objdiff/report.json")
    parser.add_argument("--symbols", default="build/gen/symbol_names.csv")
    parser.add_argument("--out", default="build/gen/reloc_aliases.tsv")
    args = parser.parse_args(argv)
    proofs, coverage, authorized_sites = collect_proofs(
        args.base_dir, args.target_dir, args.report, args.symbols)
    aliases = choose_aliases(proofs)
    write_aliases(args.out, aliases)
    alternative_targets = sum(alias.alternatives > 0 for alias in aliases.values())
    print("reloc aliases: %d exact target RVAs from %d authorized sites; "
          "%d target RVAs had alternate owner spellings" % (
              len(aliases), authorized_sites, alternative_targets))
    print("coverage: exact_functions=%d paired_functions=%d base_sites=%d "
          "same_site_same_type=%d missing_target_site=%d type_mismatch=%d "
          "unresolved_base=%d unresolved_target=%d duplicate_string_ambiguity=%d "
          "unknown_compiler_local=%d" % (
              coverage.exact_functions, coverage.paired_functions,
              coverage.base_sites, coverage.same_site_same_type,
              coverage.missing_target_site, coverage.type_mismatch,
              coverage.unresolved_base, coverage.unresolved_target,
              coverage.duplicate_string_ambiguity,
              coverage.unknown_compiler_local))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
