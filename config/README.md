# Configuration

Build contracts live here; retail facts live under `retail/`, reviews under
`reviews/`. Every file is read by the named tooling. Generated state belongs
in `build/`.

## Build and scoring

- `units.toml`: per-TU manifest. `[flags]` profiles hold the complete VC6 SP5
  command line; each `[[unit]]` selects one source and profile, in link order.
- `match_baseline.tsv`: the hash-scoped MAX ledger (`homm2 status update`).
- `link_diff.tsv`: the linked image's per-region ceiling against retail
  (`python3 -m homm2.build.link_diff --update`); every region is 0.

## Retail facts (`retail/`)

Files at the top of `retail/` describe the pinned Buka `HMM2PL.exe`; a second
image keeps the same kinds of files in its own subdirectory (`retail/editor/`
for `EDT2PL.exe`). Addresses are image RVAs.

- `targets.json`: sizes, hashes and staging options of both executables.
- `functions.csv`: the function-start inventory (analysis output, reviewed);
  a start becomes a claim only through a source `VA(...)`.
- `functions_static_libs.csv`: CRT bodies identified against the VC6 archives;
  `functions_imports.csv`: import thunks; `functions_eh.csv`: C++ EH funclets;
  `functions_compgen.csv`: compiler-generated functions
  (`homm2 audit unmatched-census --write-config`).
- `absolute_relocations.tsv`: the DIR32 site manifest that stands in for the
  stripped base-relocation directory (`homm2 audit reloc-sweep --write`), with
  the reviewed overrides `reloc_inclusions.tsv` and `reloc_exclusions.tsv`.
- `reloc_aliases.tsv`, `reloc_rel32_aliases.tsv`: reviewed relocation
  identities the delinker cannot infer; `reloc_data_owners.tsv`: public data
  extents proven independently of the next symbol; `text_exclusions.csv`:
  reviewed non-procedure `.text` ranges.
- `data_compgen.tsv`: compiler-generated data claims;
  `data_initialized_storage.tsv`: reviewed initialized storage for the strict
  allocation audit.
- `link_order_crt.txt`: the reviewed LIBCMT member order of the retail link.

## Reviews (`reviews/`)

- `bool_exceptions.tsv`, `cast_exceptions.tsv`: reviewed exceptions of the
  Boolean-field and cast audits (`homm2 audit bool-fields`, `homm2 audit
  casts`).
