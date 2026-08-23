# Static-storage link audit

`ninja link` writes a per-symbol static-storage audit to
`build/link/HMM2PL.link.json` under `static_storage.public_symbols`. This is a
final-link diagnostic, not an objdiff data-matching score and not a mechanism for
forcing retail addresses.

The same report compares the complete raw `.rdata` and `.data` payloads at
section-relative offsets under `static_storage.section_bytes`. Exactness, SHA-256
identities, equal/mismatched byte counts, and the first contiguous mismatch ranges
are recorded. This whole-section comparison is the final authority: a normalized
object-level 100% score is not proof when either linked section differs. Per-symbol
and contribution diagnostics exist to attribute the first whole-section mismatch.

`subbands` applies every candidate MAP contribution boundary to the same offset in
the equal-RVA retail section and compares that bounded span independently. This keeps
linker-owned import arrays, ordinary read-only data, exception metadata, initialized
data, and alignment tails separate. The retail image has no MAP, so these are explicitly
candidate-boundary projections; the raw whole-section result remains authoritative.

`python3 -m homm2.build.assert_relocs --pe-data` supplies the complementary
code-site audit for all uniquely identifiable configured functions. Its exhaustive
pass reads every retail DIR32 operand from the shipping PE and compares the resulting
`.rdata`/`.data` identity multiset with candidate identities, independent of code
site or instruction order. Candidate compiler-local names resolve only through
reviewed COFF coordinates. Compiler strings normally acquire those coordinates from
the exact relocation/addend oracle; unique relocation-target content is the fail-closed
fallback, and ambiguous content requires an explicit source pin. The candidate PE
independently confirms that each operand reaches one of the audited sections. Thus an
equal anonymous payload at another retail address cannot satisfy the check.

This is semantic matching, not a requirement to annotate every compiler allocation.
`DATA_COMPGEN` is retained only where content, relocation/addend structure, and use
sites cannot distinguish an otherwise justified owner. Raw `_$E<n>`/`_$S<n>` names
are not stable identities across compilations and are therefore checked as one
unit-level relocation group rather than paired by their numeric suffix. Semantic
`__h2cg$...` identities, optional MAP owner prefixes, and externally folded COMDATs
are resolved before the final-image audit.

A second, ordered pass compares final section offsets only where a small
relocation-masked code window agrees. Ordered identity diagnostics are further
limited to functions with equal data-reference counts, preventing unrelated
instructions in structurally different functions from being paired by numeric
offset alone.

The command reports two independent classes. `divergences` are final linked
section-offset differences and therefore expose cumulative allocation/link order.
`identity_divergences` mean the ordered candidate relocation names a different
retail allocation at the same code site. Balanced transpositions are reported
separately: they still differ in ordered relocation identity, but require
instruction semantics to decide whether they are merely commutative operand
ordering.

For every entry in `config/required_initialized_storage.tsv`, the link gate also
requires the current source `DATA()` allocation size reported by Clang to equal
the reviewed retail extent. Hashing only the reviewed prefix is insufficient: a
short array followed by zero-valued storage can otherwise reproduce the expected
prefix while remaining out of bounds at runtime.

An enrollment may also provide readable_size and readable_sha256 when retail code
intentionally reads beyond the source allocation into evidenced adjacent storage.
The allocation size remains checked against sizeof; the larger readable span is
independently compared in both linked images so a later storage reorder cannot change
the overread bytes silently.

## Evidence and classification

The retail inventory is limited to source-claimed `data` rows from
`build/gen/symbol_names.csv`, classified as data by their retail address. Synthetic
`pe-reloc-constant` rows are deliberately excluded.

Synthetic PDB procedure records used elsewhere in the reconstruction are not
evidence for this audit and are not read.

Retail storage classes come from the shipping PE:

- `.rdata` is read-only initialized storage.
- The raw-backed prefix of `.data` is initialized writable storage.
- The virtual portion of `.data` beyond its raw size is loader-zero storage.

The final all-zero bytes physically present in raw `.data` can be linker file-alignment
padding that overlaps the start of the original BSS contribution. The public-symbol audit
classifies such a symbol as `data-loader-zero-padding` only when its retail RVA is wholly in
that final zero tail and the candidate MAP independently places the allocation in `.bss` or
`<common>`. This preserves the ambiguity in the PE alone while avoiding a false initialized-
storage mismatch when the object-level evidence resolves it.

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

## Limits

The retail executable has no MAP and is fully stripped, so nothing retains the
original object-level `.bss` versus common allocation choice. The retail PE proves
only raw-backed initialized bytes and a loader-zero virtual tail. File alignment can
also make the last raw-backed page larger than the meaningful initializer payload.

The candidate MAP exposes only symbols emitted by the current link. Private static
objects without a public name cannot be correlated individually from the MAP alone;
reviewed retail code operands can still recover their identities. An exact symbol RVA
and class also does not prove that all bytes or pointer relocations within the object
match retail; those still require an initializer and relocation audit against the
shipping PE.

The runtime inputs are the VC6 SP5 toolchain's `MSVCPRT.LIB` early delete scan
followed by its ordinary `LIBCMT.LIB` scan. Current section sizes and the first
relative divergence are recorded in `build/link/HMM2PL.link.json` rather than
copied into this durable document.

At the exact-function checkpoint, the report contains 1,727 configured functions.
It audits all 1,530 stable semantic identities individually and records 197 volatile
counter-named helpers as excluded from individual pairing; the separate unit-level
gate checks their complete relocation groups. The individual pass scans 25,384 DIR32
sites; its `.rdata`/`.data` multisets contain 22,384 relevant retail sites and 22,384
candidate sites, all matched. There are no identity substitutions, balanced
transpositions, relocation-shape differences, candidate excesses, novel identities,
unresolved identities, or unavailable stable functions. All 240 candidate import
symbols also resolve to the same semantic DLL/name-or-ordinal identities as retail.

The ordered final-placement pass still records 6,154 shifted references in 501
functions: 349 into `.rdata` and 5,805 into `.data`. Those are cumulative linked
allocation-order differences, not evidence of different referenced data. For example,
`cFRDummy` has the exact source and object relocation to the semantic empty-string
owner, while that private string occupies a different candidate `.data` offset.
Raw whole-section and subband differences remain authoritative layout residuals in
`build/link/HMM2PL.link.json`; semantic identity matching explains them but does not
erase or declare their bytes exact.

The linked-section residuals are partitioned and bounded in
`docs/linked-data-section-walls.md`. In particular, automatic compiler-data
identity proves the reconstructed owners without requiring a marker for every
private literal. The residual ledger keeps raw PE bytes, semantic identities,
volatile linker metadata, import order, and CRT archive order as separate facts.

## Recovered source and model divergences

Widening the audit from the former near-exact/site-aligned subset to all functions
exposed these genuine reconstruction errors. Each correction is supported by the
shipping operand and the value at its retail destination, not by fuzzy score or a
candidate-derived target object:

| Area | Former reconstruction | Retail evidence and correction |
| --- | --- | --- |
| HERO secondary-skill labels | Indexed `gSecondarySkillLevels` by the stored 1-based level. | Both operands use array base minus one; index by `level - 1`. This removes an out-of-range access for the highest level. |
| PHILAI position scoring | Distance base `1.0`, strategic base `2.0f`, and land multipliers `1.4/1.5/2.0/2.5/4.0`. | Retail pool operands select `0.2`, `1.0f`, and `1.2/1.4/1.7/2.0/2.5`; the far multiplier remains `3.0f`. |
| PHILAI event/town scoring | Danger `0.9`, transfer `0.05`, third-week share `0.75`, travel-gate scale `0.75`, campfire resource amount `4`, magic-garden gold `175`, and town-event base `1.0`. | Direct operands select `0.2`, `0.04`, `0.8`, `0.85`, `5`, `500`, and `0.9`, respectively. The event-human multiplier is the retail double `1.5`; the target-human boost remains the separate float `1.5f`. |
| PHILAI magic-garden resource index | Used `gafAITurnCostResource[metadata]`. | Retail addresses the table at base minus four with the metadata index; use `metadata - 1`. |
| SPELLAI full-effect constant | A retained `static const float` made five assignments load storage. | Retail encodes those assignments as immediate `0x3f800000` and retains one anonymous pool value only for floating use; a macro reproduces both code and the exact SPELLAI `.rdata` pool. |
| `iconf2by` private BSS identities | Paired 18 identical zero-filled owners to retail addresses by candidate section order. | Repeated setup and decoder operands recover every semantic owner-to-RVA edge. Correcting the permutation raises the exhaustive identity match by 66 occurrences and removes all candidate-only targets from the function. |

Several additional edits recover retail expression factoring without changing game
semantics: EVENTS spells use `gSpellNames[cell->m_objectMetadata - 1]` directly;
FINDPATH recomputes both diagonal neighbor coordinates from `normalDirTable`; and
its binary-search midpoint/condition use retail's shift and operand spelling. They
remove false relocation-shape or identity residuals but are not counted as gameplay
bug fixes.
