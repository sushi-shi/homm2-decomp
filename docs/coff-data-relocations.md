# Whole-object COFF data relocation topology

`homm2 data-relocs` compares the candidate base COFF object with its delinked target
without using objdiff allocation boundaries. It parses raw i386 COFF tables and emits a
machine-readable census of every section, primary symbol, and data-section relocation.
This is distinct from `strict_allocation_diff.py`: the allocation audit proves selected
payload extents, while this verifier covers the complete object-level data relocation
topology.

```sh
homm2 data-relocs build/objdiff/base/SOURCE/GAME.obj \
  build/delink/SOURCE/GAME.c.obj --unit SOURCE/GAME \
  -o build/data-relocs/GAME.json

homm2 data-relocs --manifest build/data-reloc-pairs.json \
  --homm2-root . -o build/data-relocs/report.json
```

A batch manifest has schema 1 and a `pairs` array. Every row contains `unit`, `base`, and
`target`. The command exits nonzero for residuals, provenance errors, forbidden target
identities, or parse failures.

```json
{
  "schema": 1,
  "pairs": [
    {
      "unit": "SOURCE/GAME",
      "base": "build/objdiff/base/SOURCE/GAME.obj",
      "target": "build/delink/SOURCE/GAME.c.obj"
    }
  ]
}
```

## Compared topology

Each relocation retains its source section name/class, section-relative offset, i386
relocation type, raw field bytes and signed implicit addend, referenced symbol identity,
defined/undefined/common/absolute status, storage class, target section class, and a
resolved owner identity plus owner-relative addend. Duplicate relocation records are
counted rather than collapsed. Residuals are grouped per TU and site as missing, extra,
multiplicity, or property mismatches, with both original and normalized records present.

Default scope is initialized and uninitialized data sections. `--include-code` expands the
census to every relocation section for diagnostics; it does not change comparison rules.
Identity/provenance policy always scans the complete target symbol and relocation tables,
so a code-only `empty_stub` or fallback symbol cannot escape a default data comparison.

## Identity mappings

An optional schema-1 mapping file maps target names to base names. A string value is a
proven mapping. Object form records status and an optional addend adjustment:

```json
{
  "schema": 1,
  "symbol_mappings": {
    "retail_name": "base_name",
    "candidate_name": {
      "base_name": "base_owner",
      "status": "provisional",
      "addend_adjustment": 4,
      "provenance": "candidate-COFF"
    }
  },
  "section_mappings": { ".retail": ".data" }
}
```

Proven mappings participate in normalization. Provisional mappings never hide an identity
or addend mismatch: the residual remains and carries the mapping record for the next
topology correction.

Target names `const_*`, `string_*`, `data_*`, `bss_*`, `empty_stub`, metadata aliases,
and unresolved/fallback names are unconditional errors even if a mapping mentions them.

## HoMM2 provenance

`--homm2-root .` reads the shared Clang `VarDecl` inventory used by the data-manifest
adapter and cross-checks it against `build/gen/delink_data_from_source.tsv`. Compiler and
linker allocations without a source annotation live only in
`config/delink_data_supplemental.tsv`. The generated
`build/gen/delink_data_manifest.tsv` must be exactly the conflict-free union of those two
inputs: missing rows, extra rows, duplicate identities/RVAs, overlaps, malformed schemas,
and source-inventory drift are hard provenance diagnostics.

Anchors are indexed by translation unit. A relocation resolves against its current unit
first, which preserves private identities and repeated compiler names. Cross-unit lookup
is allowed only for an external symbol identity that occurs exactly once in the merged
manifest; repeated or private names are never guessed globally. These checks validate
provenance and do not grant normalization permission.

## Empty return stubs

The pinned Vostok delinker retains real PDB identities for byte-identical function groups
by default, including multiple legitimate aliases at the same COFF offset. Cross-object
calls therefore reference a real canonical name. Legacy byte-pattern names (`empty_stub`,
`identity`, `vec_begin`, and `vec_size`) exist only behind
`vostok-delinker --coalesce-common-functions`; canonical HoMM2 regeneration never passes it.
