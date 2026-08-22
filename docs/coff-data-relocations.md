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

`--homm2-root .` reads the shared `DATA`, `DATA_COMPGEN`, `VTBL`, and `VTBL2`
inventories used by the data-manifest adapter and cross-checks them against
`build/gen/delink_data_from_source.tsv`. That manifest additionally contains
source-free compiler strings admitted by exact relocation/addend proof or the fail-closed
unique relocation-target payload oracle. The generated
`build/gen/delink_data_manifest.tsv` must contain exactly the same reviewed definitions
projected into Vostok's schema. Missing rows, extra rows, duplicate identities/RVAs,
overlaps, malformed schemas, source-inventory drift, and unrecognized automatic
provenance are hard diagnostics.

Anchors are indexed by translation unit. A relocation resolves against its current unit
first, which preserves private identities and repeated compiler names. Cross-unit lookup
is allowed only for an external symbol identity that occurs exactly once in the merged
manifest, or whose repeated folded-COMDAT rows all name the same reviewed RVA, extent,
and external scope. A disagreement or any private repetition remains ambiguous rather
than being guessed globally. These checks validate provenance and do not grant
normalization permission.

## Empty return stubs

The pinned Vostok delinker retains real PDB identities for byte-identical function groups
by default, including multiple legitimate aliases at the same COFF offset. Cross-object
calls therefore reference a real canonical name. Legacy byte-pattern names (`empty_stub`,
`identity`, `vec_begin`, and `vec_size`) exist only behind
`vostok-delinker --coalesce-common-functions`; canonical HoMM2 regeneration never passes it.
