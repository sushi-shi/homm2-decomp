# Static-storage link audit

`ninja link` writes a per-symbol static-storage audit to
`build/link/HEROES2W.link.json` under `static_storage.public_symbols`. This is a
link-layout diagnostic, not an objdiff data-matching score and not a mechanism for
forcing retail addresses.

## Evidence and classification

The retail inventory is limited to `data,cv-public-data` rows from
`build/gen/symbol_names.csv`. These are public symbols retained in the embedded
minimal NB09 stream and classified as data by their retail address. Synthetic
`pe-reloc-constant` rows are deliberately excluded.

The `cv-public-data` label is a project-side classification of shipping `S_PUB32`
records, not a distinct CodeView data record. Synthetic PDB procedure records used
elsewhere in the reconstruction are not evidence for this audit and are not read.

Retail storage classes come from the shipping PE:

- `.rdata` is read-only initialized storage.
- The raw-backed prefix of `.data` is initialized writable storage.
- The virtual portion of `.data` beyond its raw size is loader-zero storage.

Candidate symbols are joined by exact decorated name to the Microsoft LINK MAP.
The containing MAP contribution classifies them as `.rdata`, initialized `.data`,
or `.bss`/`<common>` loader-zero storage. Missing and multiply defined names remain
explicit instead of being matched heuristically.

## Reading divergences

The report retains every public data-symbol row, aggregate counts, and constant
displacement runs in retail RVA order. `first_divergences` reports the start of a
run, a storage-class transition, or a missing/ambiguous symbol. Later symbols with
the same RVA delta and storage-class pair are cumulative layout consequences; they
are not independent requests for padding, address annotations, or linker tricks.

Fix the earliest credible cause, relink, and inspect the new run boundaries. Source
definitions must continue to model the proven initializer and storage class. Do not
add giant padding objects or overlapping storage to reproduce a final-image gap.
The byte and owner analysis for the first `.rdata` transition is recorded in
[`rdata-contribution-audit.md`](rdata-contribution-audit.md).

## Limits

The retail executable has no MAP and its minimal NB09 payload does not retain the
original object-level `.bss` versus common allocation choice. The retail PE proves
only raw-backed initialized bytes and a loader-zero virtual tail. File alignment can
also make the last raw-backed page larger than the meaningful initializer payload.

The candidate MAP exposes only symbols emitted by the current link. Private static
objects without a public name cannot be correlated individually. An exact symbol RVA
and class also does not prove that all bytes or pointer relocations within the object
match retail; those still require an initializer and relocation audit against the
shipping PE.
