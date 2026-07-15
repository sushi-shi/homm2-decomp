# Reviewed public-data targets

The native objdiff 3.7.1 report exposes data at section granularity. It does not
produce one report row per COFF data symbol, and the synthetic PDB cannot fix that:
CodeView `S_LDATA32`/`S_GDATA32` records carry a name, type index, and address, but
no allocation length or initialized-versus-loader-zero storage classification.

`config/required_initialized_storage.tsv` is therefore the sole exact-extent input
to reviewed target data. `scripts/homm2/build/reviewed_data.py` joins each enrolled
name to the public inventory to validate address and compiland ownership, checks its
retail PE storage and payload hash, and writes
`build/gen/reviewed_delink_data.tsv`. The generated manifest labels every extent as
`reviewed-required-initialized-storage`; it never promotes the provisional
next-public gap in `build/gen/symbol_names.csv` to CodeView truth.

The patched delinker consumes the project-neutral columns `name`, `object`, `rva`,
`size`, `storage`, `alignment`, and `provenance` through `--data-manifest`. It emits
each full definition in its explicitly named target object, preserves the manifest
`.data`/`.rdata`/`.bss` class and alignment, and converts base relocations inside the
definition to COFF relocations. Function
references to enrolled definitions become externals instead of creating duplicated
four-byte target allocations.

`homm2 status` hashes the ledger, public inventory, retail EXE, synthetic PDB, and
delinker executable. A changed input triggers a focused delink into a temporary
directory and replaces `build/delink` only after every reviewed owner object exists.
This refresh does not regenerate the inventory, synthesize a PDB, or rerun the full
`homm2 init` pipeline. Init records the same stamp after its normal delink, so the
first status run does not repeat that work.

The delinker manifest and parser are project-neutral. Only the HoMM2 adapter knows
about NB09 and `required_initialized_storage.tsv`, so another reconstruction project
can generate the same generic manifest from its own reviewed evidence. Per-symbol
allocation scoring belongs in the generic objdiff consumer; this adapter does not
rewrite objdiff's native section measures. The final-link initialized-storage audit
remains authoritative for PE pointer-target content and final storage.
