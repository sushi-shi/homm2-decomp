# Missing public-data link audit

The strict final link writes `build/link/HEROES2W.missing-data.tsv` and the equivalent
JSON list at `static_storage.public_symbols.missing_symbols` in
`HEROES2W.link.json`. Each row contains the retail `S_PUB32` name, owning compiland,
RVA, public-symbol span, retail PE storage class, root-cause classification, and
supporting evidence.

Candidate correlation uses exact decorated MAP names. Microsoft LINK renders
non-printable bytes in MAP symbol names as octal escapes, so the parser decodes those
escapes before comparing. In particular, `\177..._NULL_THUNK_DATA` is the printable
MAP spelling of a symbol whose first byte is `0x7f`; it is not a missing definition.

The remaining root-cause classes are:

- `project-definition-absent`: a public owned by a reconstructed SOURCE, BASE, or
  EDITOR unit has no candidate definition. These are actionable only after confirming
  the retail type, storage class, initializer, and owner.
- `unreferenced-system-import`: a retail IAT public is absent. Audit the source call
  and its relocation before changing the import set.
- `runtime-library-private-literal`: compiler-generated data owned by a retail CRT
  compiland differs because the selected runtime member or its implementation differs.
  Do not recreate these literals as project globals.
- `legacy-import-library-bookkeeping-name`: old import-library machinery and the
  generated replacement use different descriptor-public names. These are linker-owned
  records, not source storage and not candidates for aliases.
- `unsupported-or-unlinked-owner`: no exact definition or proven equivalent is known.

The `size` column is the public-symbol span from the retail inventory, not necessarily
`sizeof` the source object. Do not infer array bounds from it without checking the type,
neighboring symbols, and retail bytes.

The JSON symbol rows also record `section_relative_delta`. This subtracts the retail and
candidate section RVAs before comparing a public's position, so a whole-section shift does
not masquerade as an object-contribution mismatch. `first_section_relative_divergences`
lists only the first public at each new nonzero relative delta within a section.
