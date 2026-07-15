# Strict data-allocation audits

The patched `objdiff-cli diff` JSON exposes the section index and data relocation
rows for every symbol. `scripts/strict_allocation_diff.py` uses those raw side
records to audit reviewed allocations without relying on objdiff's fuzzy score.
The format and checker are project-neutral and work with any objdiff project.

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
      "section_kind": "SECTION_DATA"
    }
  ]
}
```

`base_name` may be placed on an allocation when it should not use the global
symbol mapping. `section_kind` is optional; when present it is one of
`SECTION_DATA`, `SECTION_BSS`, or `SECTION_COMMON`.

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
