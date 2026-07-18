# Candidate COFF data topology

`python3 -m homm2.build.candidate_data_manifest` inventories definitions directly from each
compiled candidate object. Candidate COFF proves symbol names, local/external scope, storage
class, section-relative topology, alignment, and relocation spelling. Retail PE bytes and NB09
public symbol RVAs and `sstModule` contribution ranges remain authoritative for addresses and
contents. The shipping NB09 stream has no game procedure, local, type, or line records; this
process does not infer any of those from CodeView.

The primary mapper replays candidate sections into independent `.rdata`, `.data`, and `.bss`
streams in COFF section-table order with their encoded alignment. A retail contribution start plus
the replayed section/symbol offset derives private RVAs directly. NB09 public RVAs are hard anchors;
candidate DIR32 versus retail HIGHLOW sites and literal payload bytes validate identity. When the
current source has a real stream-layout divergence, the individual-evidence mapper records only a
hard public RVA, a unique aligned relocation proof, or a unique literal payload occurrence.
Ambiguity, overlap, a missing definition, or an uncovered reference leaves the whole group open.
Those individual placements may enter the review queue with their contradictions, but never a
canonical manifest or target.

This proves allocation identity and placement; it does not by itself prove which
allocation a particular instruction uses. Two identical constants can both have
valid retail placements while a candidate code relocation selects the wrong one.
The direct retail-PE operand audit (`assert_relocs --pe-data`) is the authority for
that code-site relationship. PHILAI's former `1.1f` reconstruction was the
regression fixture: payload/topology paired equal `1.1f` bytes elsewhere in the
retail pool, whereas this retail instruction operand selected the retained
`1.5f` owner. The synthesized delinked target is not an original retail object
and cannot establish that retail emitted an additional dead slot.

Linker-sorted `.CRT$...` subsections do not participate in ordinary `.data` cursor replay. The
mapper assigns one only when the same owner has exactly one physical retail contribution with the
same writable domain and size, then removes that contribution from ordinary replay. A missing or
ambiguous contribution leaves the subsection explicitly unassigned.

The direct candidate-tool manifest records closed-group section offsets and symbol scope as
discovery evidence only. It is not reviewed and is never a delinker input in the canonical path.
The pinned delinker consumes the separate source-DATA-plus-reviewed-supplemental manifest, resolves
interior references as owner plus addend, and errors if a relocation reaches an uncovered address.
Undefined candidate externs remain undefined; retail storage is never copied into the referring
object.

The compatibility option `--recover-data-relocs-from-pdb` retains permissive nearest-PDB-symbol
recovery. It now preserves read-only `.rdata`, initialized `.data`, and zero-fill `.bss` storage;
it can still produce synthetic `const_` or string identities and is not proof for canonical data
ownership.

`--require-all` turns the inventory command into a discovery-closure gate: it writes no manifest
and exits unsuccessfully if any candidate data-bearing group remains open. Without that flag the
tool emits only independently closed groups for diagnostics. That partial output is neither a
canonical input nor evidence that the program-wide topology is complete.
`--diagnostics-output` always writes a stable JSON queue, including overlapping cause and storage
counts, even when `--require-all` refuses to write the manifest.

## Target lifecycle

`homm2 init` has two distinct modes. Before canonical manifests exist, it creates a permissive
bootstrap target using `--recover-data-relocs-from-pdb`; that target exists only so candidate
objects can be compiled and inspected. Once source DATA and the reviewed supplemental manifest are
available, init assembles their exact generated union and reproduces the strict target from it.

The explicit topology commands are:

- `homm2 data-topology audit` is the normal iterative entrypoint. It runs Ninja's parallel,
  incremental `all` target to refresh candidate COFFs, assembles the canonical source-DATA plus
  reviewed-supplement inputs, derives the review queue once, and refreshes whole-image coverage.
  It never regenerates or replaces target objects. `--jobs N` controls Ninja parallelism;
  `--strict` fails while any symbol group, section assignment, or coverage diagnostic remains.
  MSVC compiler-local counters are not canonical identities. A counter-only `$SG`, `$T`, or
  `name$S<number>` drift is accepted without rewriting the reviewed manifest only when the fixed
  target and current candidate have the same private-symbol family at the exact section
  ordinal/offset and canonical payload/relocation identity. Structural or content changes still
  fail and require review.
- `homm2 data-topology census` compares every configured candidate object with its fixed delinked
  target and writes `build/gen/data_topology_census.json`. It treats symbol names and complete COFF
  topology records as multisets, so duplicated target symbols remain visible. `missing` means an
  expected candidate identity is absent from the target; `extra` means a target-only identity. The
  report includes per-object and whole-tree common/union counts and both lists with multiplicity,
  defined/common/undefined state, data section, local/global scope, raw storage-class mismatches,
  and `const_`/`string_`/`data_`/`bss_` inventories. Exact topology records are proved mappings;
  same-name records with different definition/scope/storage topology are reported separately as
  provisional real mappings. These are review diagnostics only; the canonical symbol manifest and
  target may contain only source DATA and explicitly reviewed supplemental rows. `const_`,
  `string_`, `data_`, `bss_`, `empty_stub`, and `[section-N]` fallback identities are always hard
  errors.

  Symbol equality is not sufficient for an exact object. A separate raw COFF data-section
  multiset retains section-table ordinal/order, duplicate raw names such as Midi's COMDAT `.data`
  contributions, size, characteristics, decoded alignment, COMDAT selection/associative parent,
  relocation count, and section-definition symbol plus auxiliary metadata. It never uses objdiff's
  synthesized `[.data-N]` display labels. Exact-object status requires both symbol and section
  topology equality; section drift is also a strict hard error. The default iterative mode exits
  successfully after writing all available diagnostics; `--strict` fails on a missing/invalid
  object, any target fallback identity, or any real identity without a counterpart. Input roots,
  suffixes, unit manifest, and output path are command-line options so the same COFF audit can be
  run against another build tree.

  The same JSON keeps allocation provenance separate. A shared Clang `VarDecl` inventory attaches
  each token-aware `DATA()` marker to exactly one definition and records its logical `sizeof`.
  Candidate definitions which map to those source names are reported as DATA-covered; remaining
  candidate definitions are compiler-private derived topology. Rows in
  `config/delink_data_supplemental.tsv` are supplemental linker metadata only. A supplemental row which
  repeats a canonical DATA allocation, or disagrees with its owner/RVA/storage evidence, is a hard
  error rather than a second definition. Normal assembly preserves reviewed supplemental rows
  byte-for-byte. Semantic/public names remain exact. For local compiler-private counters only, an
  exact-coordinate fallback additionally requires unchanged size/storage/alignment/scope and a
  matching canonical payload plus relocation identity against the fixed reviewed target object.
  Any unproved rename or topology/content change fails. Explicit `--migrate-from` remains available
  for a real reviewed identity/topology change, whose versioned output must be reviewed before use.
  Ordinary COFF symbols do not carry logical sizes, so a reviewed size may be smaller than the
  physical span to the next symbol; assembly preserves that size and requires it to fit the span.
  `--source-root`, `--supplemental`, and `--symbols` select these inputs for another tree.
- `homm2 data-topology assemble` creates all HoMM2-specific Vostok inputs. It resolves every source
  definition to one exact candidate decorated symbol, COFF section ordinal and value, scope, and
  storage class. The generated `build/gen/delink_data_from_source.tsv` and versioned
  `config/delink_data_supplemental.tsv` are validated separately, then merged without logical
  duplicates or overlaps into `build/gen/delink_data_manifest.tsv`. Logical `sizeof` is never
  replaced by a padded next-symbol span; padding exists only in section size/alignment.

  `build/gen/delink_data_sections.tsv` preserves every candidate section ordinal, including
  multiple same-name COMDAT sections, characteristics, alignment, selection, and associative
  parent. An affine section records one retail RVA used as its byte-copy source. A fully reviewed
  non-affine section records `rva=-` while retaining its storage class: Vostok creates the exact
  candidate COFF section, zero-fills gaps, and copies each enrolled definition and its relocations
  from that definition's independent reviewed retail RVA. Non-data sections remain explicit `-`
  placeholders with no storage class. Candidate-offset bounds and overlaps are hard errors.

  The companion `build/gen/delink_data_breakpoints.json` separates blocking assignment
  diagnostics from exact non-affine classifications. Candidate-order replay remains discovery
  evidence for sections without complete reviewed definitions. A unique complete reviewed base
  overrides replay and is not required to retain the candidate input section alignment: NB09
  contribution ranges exclude final-link padding. `--strict` rejects unresolved sections, not
  reviewed non-affine placement. The generated contribution and whole-image
  coverage manifests remain hard regeneration inputs under `build/gen`; they are not copied into
  version control. The Clang DATA inventory is cached under `build/gen` per TU, keyed by source,
  recursively resolved project/vendor headers, candidate object, compile database, and parser
  implementation. Missing objects disable reuse; source or header changes reparse the affected TU.
- `homm2 data-topology propose` writes only structured candidate diagnostics and
  `build/gen/data_topology_review_queue.tsv`. The queue is deliberately not a Vostok manifest: its
  header adds evidence kind, proof count, group blockers, and group contradictions. It contains
  real candidate identities absent from the canonical source-DATA-plus-supplemental union when an
  individual placement has public-RVA, aligned relocation/addend, unique literal-payload, or
  section-replay evidence. An open group does not erase that evidence, but its contradictions ride
  every proposed row. Unmapped rows remain diagnostic-only. Synthetic/fallback identities are
  filtered, and canonical identities are omitted. `row_kind=allocation-symbol` makes explicit that
  the current queue contains COFF allocation symbols; aliases and section-definition metadata are
  not flattened into fake allocations and remain in topology diagnostics until separately modeled.
  A group whose every candidate definition already has an exact source-DATA or reviewed
  supplemental row is closed directly from that canonical evidence after candidate section,
  offset, storage, scope, alignment, and extent validation. This permits a reviewed retail owner
  order to differ from candidate allocation order without asking for the same rows again.

  Proposal does not refresh coverage, source-DATA, combined symbol/section, contribution, or target
  artifacts. The queue is review input only: it is never read by promote, finalize, regenerate, or
  the delinker, and its rows are never auto-promoted.
- `homm2 data-topology promote` refreshes diagnostics and coverage evidence, then regenerates the
  canonical symbol/section manifests strictly from source `DATA()` definitions plus the explicitly
  reviewed `config/delink_data_supplemental.tsv`. Candidate facts and the review queue cannot enter
  that union. Promote does not replace target objects.
- `homm2 data-topology finalize` is the program-wide closure gate. It writes no canonical manifest
  unless every candidate group is closed and the `.text`, retail file, loaded-RVA, and TU-data
  partition audits have zero blockers.
- The only versioned storage-topology supplement is `config/delink_data_supplemental.tsv`; the
  source tree is the canonical input for annotated definitions. Reviewed COFF relocation spellings
  are independent and live in `config/delink_reloc_aliases.tsv`. The three text-delink CSVs remain
  independent.
- `homm2 data-topology regenerate` deterministically rebuilds the generated inputs and atomically
  replaces `build/delink`. Its provenance stamp hashes every config, the retail executable, the
  synthetic delinker-input PDB, and the pinned delinker executable. Its data symbols come only from
  source `DATA()` plus reviewed supplemental rows; it never reads the review queue or the legacy
  direct-tool `candidate_delink_data.tsv` output. Proposal evidence comes from candidate COFF,
  retail PE/contribution bytes, NB09 public RVAs, and retail relocations, never from the old delinked
  target's `const_*` or `empty_stub` identities.
  It passes the reviewed relocation-alias config directly to Vostok; candidate-derived alias
  proposals under `build/gen/` are never canonical input.

Normal `homm2 build` and `homm2 status` consume the fixed target. They never run the candidate
mapper, rewrite a topology config, or refresh target objects. If a canonical input changes, they
fail with an explicit regeneration instruction.

Canonical regeneration has no unresolved-data fallback. Every emitted data relocation must resolve
through the generated symbol and section manifests.

The coverage manifest contains independent exact partitions. `file` covers every byte from offset
zero through the end of the appended debug payload, including headers, raw sections, alignment,
FPO/MISC records, and the embedded NB09 stream. `loaded` covers RVA zero through `SizeOfImage`,
including every section and virtual alignment/BSS tail. `text` is emitted only by the existing
fail-closed procedure/jump-table/exclusion/padding audit. `tu-data` partitions every configured
`.rdata`, initialized `.data`, and zero-fill `.bss` contribution into literal, constant, allocation,
or explicit zero-padding records. Nonzero gaps, overlaps, cross-owner allocations, unowned publics,
invalid HIGHLOW sites/targets, or any text evidence failure block promotion.
Canonical coverage consumes the same generated, candidate-classified contribution manifest passed
to Vostok; it never silently reverts to the raw PE-boundary split during promotion.

## Checkpoint census

The recorded candidate census contains 6,734 definitions: 1,232 resolve to source `DATA()` and 5,502
are non-DATA candidate allocations. The reviewed supplemental manifest covers 634 of the latter,
leaving 4,868 unreviewed definitions. This denominator includes public data, compiler-local statics,
string literals, constant pools, vtables, and other compiler-emitted definitions.

At the reviewed checkpoint, every row in the independently closed candidate-tool manifest already
exists in the canonical union, so subtracting only closed groups would produce an empty review
queue. The proposal command therefore also surfaces individually evidenced placements from open
groups while retaining their blockers. Definitions without individual candidate-plus-retail
evidence remain only in the JSON group diagnostics; the old target is never used to fill this gap.

`BASE/Icondf2b` is the regression fixture. Its 37 candidate/retail `DIR32` sites, including two
public `uDimPal` anchors, prove a bijection for all thirteen four-byte function statics. The target
now emits their exact `_gFD*$S...` names and candidate offsets in a 52-byte BSS section, replacing
the former `const_001381xx` and unrelated string identities. Its focused score changes from
80.283134% to 81.06626% under `data_value` and to 81.03615% under `name_address`; the ordinary code
score remains 81.1265%.
