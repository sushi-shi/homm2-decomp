# Reviewed public-data targets

The native objdiff 3.7.1 report exposes data at section granularity. It does not
produce one report row per COFF data symbol, and the synthetic PDB cannot fix that:
CodeView `S_LDATA32`/`S_GDATA32` records carry a name, type index, and address, but
no allocation length or initialized-versus-loader-zero storage classification.

Two reviewed ledgers provide exact extents. `config/required_initialized_storage.tsv`
records allocations whose initializer payload has been audited. The narrower
`config/reloc_data_owners.tsv` records public owner regions whose source `DATA()` RVA
and type-derived size are independently proven, including loader-zero `.bss` storage.
`scripts/homm2/build/reviewed_data.py` joins both to the public inventory, validates
address, compiland ownership, and PE storage, and writes
`build/gen/reviewed_delink_data.tsv`. Initialized rows additionally validate the retail
payload hash and relocation count. The generator never promotes a provisional
next-public gap in `build/gen/symbol_names.csv` to CodeView truth.

The patched delinker consumes the project-neutral columns `name`, `object`, `rva`,
`size`, `storage`, `alignment`, `section_offset`, `scope`, and `provenance` through
`--data-manifest`. It emits
each full definition in its explicitly named target object, preserves the manifest
`.data`/`.rdata`/`.bss` class and alignment, and converts base relocations inside the
definition to COFF relocations. Function
references to enrolled definitions become externals instead of creating duplicated
four-byte target allocations.

The header and nine-column row shape are exact. A numeric `section_offset` enrolls the
row in a candidate-topology group; `-` preserves the legacy reviewed allocation form.
`scope` is `local` or `external` and controls the emitted COFF symbol scope. Names,
object paths, and provenance
must be non-empty and contain no control bytes. Object paths are normalized relative
paths; absolute, drive-qualified, UNC, empty-component, and parent-component paths
are rejected. Names and RVAs are globally unique, extents must be non-zero,
non-overlapping, and non-overflowing, and alignment must be a non-zero power of two.
These checks are generic delinker input validation and do not depend on HoMM2 names.

The reviewed manifest is a bootstrap input, not a self-updating baseline. Normal `homm2 build` and
`homm2 status` validate and consume the existing target without deriving candidate data or invoking
the delinker. Bootstrap targets carry a bootstrap provenance stamp and remain fixed until an
explicit init.

Candidate topology snapshots are promoted separately to the versioned
`config/delink_data_topology.tsv`, `config/delink_contributions.tsv`, and the exact partition in
`config/retail_coverage.tsv`. An explicit
`homm2 data-topology regenerate` delinks into a temporary directory, verifies every owner object,
and replaces `build/delink` only after success. Its canonical stamp hashes all committed configs,
the retail EXE, synthetic delinker-input PDB, and delinker executable. Normal commands refuse a
stale canonical stamp and instruct the user to regenerate; they never rewrite configs or targets.
The versioned `config/delink_unresolved_data.tsv` is the only range in which strict partial
regeneration may retain legacy PDB fallback. `homm2 data-topology finalize` requires that inventory
and every machine-readable coverage diagnostic to reach zero.

The delinker manifest and parser are project-neutral. Only the HoMM2 adapter knows
about NB09 and `required_initialized_storage.tsv`, so another reconstruction project
can generate the same generic manifest from its own reviewed evidence. Per-symbol
allocation scoring belongs in the generic objdiff consumer; this adapter does not
rewrite objdiff's native section measures. The final-link initialized-storage audit
remains authoritative for PE pointer-target content and final storage.
