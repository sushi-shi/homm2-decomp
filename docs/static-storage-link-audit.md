# Static-storage link audit

`ninja link` writes a per-symbol static-storage audit to
`build/link/HEROES2W.link.json` under `static_storage.public_symbols`. This is a
link-layout diagnostic, not an objdiff data-matching score and not a mechanism for
forcing retail addresses.

The same report compares the complete raw `.rdata` and `.data` payloads at
section-relative offsets under `static_storage.section_bytes`. Exactness, SHA-256
identities, equal/mismatched byte counts, and the first contiguous mismatch ranges
are recorded. This whole-section comparison is the final authority: a normalized
object-level 100% score is not proof when either linked section differs. Per-symbol
and contribution diagnostics exist to attribute the first whole-section mismatch.

`python3 -m homm2.build.assert_relocs --pe-data` supplies the complementary
code-site audit for all 1,499 unique configured functions. Its exhaustive pass
reads every retail DIR32 operand from the shipping PE and compares the resulting
`.rdata`/`.data` identity multiset with candidate identities, independent of code
site or instruction order. Candidate compiler-local names resolve only through
reviewed COFF coordinates, and the candidate PE independently confirms that each
operand reaches one of the audited sections. Thus an equal anonymous payload at
another retail address cannot satisfy the check.

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

The current `.data` result illustrates the limit. Raw size is exact at `0x37000`,
while virtual size is `0x4d49c` versus retail `0x4d4b0` (20 bytes short). All 1,447
name-joined public static symbols have the same storage class, and every configured
game candidate/target `.data` and `.bss` COFF section has the same size. The final
20-byte difference is the net result of different BSS/common ordering and alignment,
not evidence for a missing 20-byte source object. For example, DRAWING's candidate
and fixed target `.data` are byte-identical `0x7f` sections, yet retail places its
two public zero globals among its private literals while the reconstructed COFF puts
them first. The retail PE operands and reviewed non-affine manifest preserve the
real addresses even though the flattened delinked object cannot reproduce that
original ordering.

The current exhaustive census covers all 1,499 unique functions and 21,664 retail
data targets: 781 into `.rdata` and 20,883 into `.data`. Candidate code has 21,380
targets; 21,307 match a retail allocation identity. There are zero equal-count
identity substitutions and zero unresolved candidate identities. The 73 unmatched
candidate occurrences all belong to six structurally incomplete functions; their
functions also have unequal reference counts, so they are shape residuals rather
than evidence that a recovered expression names a retail-unreferenced allocation.
The report retains all 357 retail-only and 73 candidate-only occurrences.
Those six functions are the five nonexact icon blitters `FlipIconToBitmap`,
`IconToBitmapColorTable`, `IconToBitmapYModify`,
`FlipIconToBitmapColorTable`, and `FlipIconToBitmapYModify`, plus
`searchArray::SeedPosition`. Their local comments and experiment ledgers account
for the remaining publication/register-liveness shapes; none has an equal-count
identity substitution.

The ordered context pass compares 12,078 sites (507 `.rdata`, 11,571 `.data`) and
rejects 1,098 shifted contexts. All 10,710 final-placement divergences are in
`.data`; `.rdata` has none. Its 20 ordered identity differences form ten balanced,
manually reviewed transpositions: four ADVMGR pairs (two `MAP_WIDTH * MAP_HEIGHT`
products and two current-enemy/current-player comparisons), one `TotalOpenTime +
TotalReadTime` pair in SMACKMGR, two center-plus-origin pairs in Viewwrld, one
current/previous player-count comparison in Wsnetwin, and two quantity/ratio
comparisons in tradpost. They remain visible because balanced identities alone do
not prove semantic equivalence.

## Recovered source divergences

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

Several additional edits recover retail expression factoring without changing game
semantics: EVENTS spells use `gSpellNames[cell->m_objectMetadata - 1]` directly;
FINDPATH recomputes both diagonal neighbor coordinates from `normalDirTable`; and
its binary-search midpoint/condition use retail's shift and operand spelling. They
remove false relocation-shape or identity residuals but are not counted as gameplay
bug fixes.
