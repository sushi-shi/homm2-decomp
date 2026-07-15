# Strict data-allocation audits

## Standalone project-neutral workflow

The patched `objdiff-cli diff` JSON exposes the section index and data relocation
rows for every symbol. `scripts/strict_allocation_diff.py` uses those raw side
records to audit reviewed allocations without relying on objdiff's fuzzy score.
The format and checker are project-neutral and work with any objdiff project.
Vostok, Gruntz, or another consumer supplies its own reviewed manifest and
one-shot objdiff evidence; this standalone path does not read HoMM2's storage
ledger, PE, symbol inventory, or build configuration.

Generate a one-shot diff for a unit, then audit it:

```sh
objdiff-cli diff -p build/objdiff -u MODULE/UNIT \
  -o build/objdiff/UNIT.allocations.json --format json
python3 scripts/strict_allocation_diff.py \
  build/objdiff/UNIT.allocations.json config/UNIT.allocations.json
```

The review manifest is external to objdiff:

```json
{
  "version": 1,
  "symbol_mappings": {
    "target_symbol": "base_symbol",
    "target_relocation": "base_relocation"
  },
  "section_mappings": {
    ".target_data": ".base_data"
  },
  "allocations": [
    {
      "target_name": "target_symbol",
      "extent": "0x28",
      "section_kind": "SECTION_DATA",
      "relocation_mappings": [
        {
          "offset": "0x10",
          "type": 6,
          "addend": 0,
          "target_name": "target_relocation",
          "base_name": "base_relocation"
        }
      ]
    }
  ]
}
```

`base_name` may be placed on an allocation when it should not use the global
symbol mapping. `section_kind` is optional; when present it is one of
`SECTION_DATA`, `SECTION_BSS`, or `SECTION_COMMON`.

`relocation_mappings` is optional. When absent, relocation names use the global
`symbol_mappings` dictionary. When present, it is a complete reviewed list for
that allocation: every relocation must appear exactly once, and the checker
requires its offset, numeric type, addend, raw target name, and raw base name to
match. This occurrence form represents distinct identities that happen to share
a symbol spelling without inferring identity from declaration or relocation
order.

The checker requires unique presence on both sides and compares the mapped symbol
name, section name and kind, extent, payload, and every relocation's
symbol-relative offset, numeric type, mapped target name, and addend. Mappings are
explicit project inputs; neither objdiff nor the checker contains ownership or
naming rules for a particular game.

COFF data symbols commonly have no encoded size. Objdiff marks the resulting
next-symbol span with `flags.size_inferred`. The checker never accepts that span
as a reviewed extent: an inferred symbol requires an explicit positive `extent`
in the manifest. The inferred span only establishes that enough serialized bytes
exist to check the reviewed range. A non-inferred symbol must agree exactly with
the reviewed extent.

Objdiff's match percentage remains useful for navigation, but it is not the audit
criterion. The core may accept same-address relocation targets and uses fuzzy
payload matching. The strict checker compares the serialized target and base
records directly.

Objdiff serializes `data_diff` as an ordered edit stream, not sparse chunks with
addresses. Payload-bearing segments concatenate in stream order and their
decoded length must equal `size`; `DIFF_INSERT`/`DIFF_DELETE` segments without a
payload belong only to the opposite side. The checker rejects malformed segment
sizes, unknown payload-free segments, negative section indices, and negative
relocation symbol indices.

Objdiff may serialize the same relocation once for each payload chunk it
overlaps. The checker collapses only byte-identical relocation records whose
copy count is exactly explained by crossed `data_diff` boundaries. An
unexplained exact duplicate remains a hard failure; differing records remain
distinct and must satisfy the normal manifest checks. The resulting unique
relocation count still has to match the reviewed manifest; HoMM2 additionally
requires its PE HIGHLOW count and offsets.

## HoMM2 adapter

This adapter is separate from the standalone interface above. `homm2
strict-allocations` converts the reviewed rows in
`config/required_initialized_storage.tsv` into one strict manifest per object
unit, runs objdiff once per unit, and invokes the project-neutral checker above.
Use `--unit SOURCE/KB` to limit a run. Generated manifests and the
machine-readable coverage report are written to `build/gen/strict-allocations/`.
Successful intermediate diff JSON is removed by default; pass `--keep-diffs`
when debugging a failed or excluded row.

Plain `bytes` rows are enrolled at their reviewed extent. Cstring pointer tables
have a stronger mapping gate because retail delinked literals and reconstructed
MSVC literals use different names. For each relocation, the adapter requires:

- the delinked target name has exactly one authoritative
  `pe-reloc-constant` record at the actual retail pointer-target RVA in
  `build/gen/symbol_names.csv`;
- the retail cstring hash equals the candidate `$SG` symbol payload;
- relocation offsets, types, and addends agree; and
- retail target RVAs and candidate names have identical equality partitions,
  with a globally bijective RVA-to-candidate mapping.

MSVC decorated literal spellings are not identities: the same `??_C@_` spelling
can describe distinct delinked constants at distinct retail RVAs. The adapter
therefore emits the occurrence mappings above after proving each raw target name
at its exact RVA and each candidate payload. It supports both decorated and
`const_*` target spellings; neither spelling nor `$SG` ordinals determine a
mapping.

No mapping is inferred from literal order alone. A table that cannot satisfy the
proof is omitted from its generated strict manifest and listed by name with the
exact reason in `report.json` and command output. The remaining subset stays a
hard audit; exclusions never relax `scripts/strict_allocation_diff.py`.

The command requires a current Ninja build and refuses to audit stale candidate
objects. It is intentionally opt-in rather than part of `homm2 build`: every
enrolled allocation is still hard-gated, while the multi-unit objdiff pass is too
expensive for the normal few-second compile loop.
