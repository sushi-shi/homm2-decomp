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
current source has a real stream-layout divergence, the exception mapper accepts only a hard public
RVA, a unique aligned relocation proof, or a unique literal payload occurrence. Ambiguity, overlap,
a missing definition, or an uncovered reference leaves the whole group open; the generator never
emits a partial closed-world group.

The generated reviewed manifest records candidate section offsets and symbol scope. The pinned
delinker emits closed groups in that topology, resolves interior references as owner plus addend,
and errors if a relocation reaches an uncovered address in a closed group. Undefined candidate
externs remain undefined; retail storage is never copied into the referring object.

The compatibility option `--recover-data-relocs-from-pdb` retains permissive nearest-PDB-symbol
recovery. It now preserves read-only `.rdata`, initialized `.data`, and zero-fill `.bss` storage;
it can still produce synthetic `const_` or string identities and is not proof for canonical data
ownership.

`--require-all` turns the inventory command into a canonical-closure gate: it writes no manifest
and exits unsuccessfully if any candidate data-bearing group remains open. Without that flag the
tool emits only independently closed groups so they can be reviewed and integrated incrementally;
that partial output is not evidence that the program-wide topology is complete.
`--diagnostics-output` always writes a stable JSON queue, including overlapping cause and storage
counts, even when `--require-all` refuses to write the manifest.

## Target lifecycle

`homm2 init` has two distinct modes. Before canonical manifests exist, it creates a permissive
bootstrap target using `--recover-data-relocs-from-pdb`; that target exists only so candidate
objects can be compiled and inspected. After canonical manifests are promoted, init reproduces the
strict target from those versioned inputs instead.

The explicit topology commands are:

- `homm2 data-topology census` compares every configured candidate object with its fixed delinked
  target and writes `build/gen/data_topology_census.json`. It treats symbol names and complete COFF
  topology records as multisets, so duplicated target symbols remain visible. `missing` means an
  expected candidate identity is absent from the target; `extra` means a target-only identity. The
  report includes per-object and whole-tree common/union counts and both lists with multiplicity,
  defined/common/undefined state, data section, local/global scope, raw storage-class mismatches,
  and `const_`/`string_`/`data_`/`bss_` inventories. Exact topology records are proved mappings;
  same-name records with different definition/scope/storage topology are reported separately as
  provisional real mappings. The canonical target may contain provisional real mappings while
  recovery is in progress, but `const_`, `string_`, `data_`, `bss_`, `empty_stub`, and
  `[section-N]` fallback identities are always hard errors.

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
  error rather than a second definition. `--source-root`, `--supplemental`, and `--symbols` select
  these inputs for another tree.
- `homm2 data-topology assemble` creates all HoMM2-specific Vostok inputs. It resolves every source
  definition to one exact candidate decorated symbol, COFF section ordinal and value, scope, and
  storage class. The generated `build/gen/delink_data_from_source.tsv` and versioned
  `config/delink_data_supplemental.tsv` are validated separately, then merged without logical
  duplicates or overlaps into `build/gen/delink_data_manifest.tsv`. Logical `sizeof` is never
  replaced by a padded next-symbol span; padding exists only in section size/alignment.

  `build/gen/delink_data_sections.tsv` preserves every candidate section ordinal, including
  multiple same-name COMDAT sections, characteristics, alignment, selection, associative parent,
  and assigned retail RVA. Non-data sections are explicit `-` placeholders. The companion
  `build/gen/delink_data_breakpoints.json` replays independent rdata/data/bss cursors in retail
  object order. Observed DATA/public anchors never change a replay cursor: every difference is an
  explicit signed drift, and missing or inconsistent section bases remain diagnostics.
  `--strict` rejects any such unresolved assignment. The generated contribution and whole-image
  coverage manifests remain hard regeneration inputs under `build/gen`; they are not copied into
  version control.
- `homm2 data-topology propose` writes a disposable partial manifest and structured diagnostics to
  `build/gen/`. It never changes versioned configuration or target objects.
- `homm2 data-topology promote` refreshes disposable topology and coverage evidence. Source DATA
  and candidate facts are regenerated rather than promoted as a second versioned definition.
- `homm2 data-topology finalize` is the program-wide closure gate. It writes nothing unless every
  candidate group is closed and the `.text`, retail file, loaded-RVA, and TU-data partition audits
  have zero blockers.
- The only versioned data-topology input is `config/delink_data_supplemental.tsv`; the source tree
  is the canonical input for annotated definitions. The three text-delink CSVs remain independent.
- `homm2 data-topology regenerate` deterministically rebuilds the generated inputs and atomically
  replaces `build/delink`. Its provenance stamp hashes every config, the retail executable, the
  synthetic delinker-input PDB, and the pinned delinker executable.

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

## Current census

At the stream-replay checkpoint, candidate objects contain 6,733 defined symbols across 167
data-bearing object/storage groups. This denominator includes public data, compiler-local statics,
string literals, constant pools, vtables, and other compiler-emitted definitions, not just the 281
source lines spelling `DATA(...) static`.

The replay plus evidence exception path proves 908 definitions in 98 complete groups. The
remaining 69 groups are reported explicitly; two formerly accepted BSS mappings are now open
because no matching sstModule contribution owns them. The open set otherwise contains the prior
47 initialized-data, 19 BSS, and one read-only-data
group. Diagnostic classes overlap because one group can fail several checks: 53 have a referenced
retail RVA not yet covered, 35 contain an unmapped candidate definition, 12 have an ambiguous
mapping, 26 produce overlapping inferred retail extents, 12 have an extent outside retail storage,
and 10 fail the one-to-one mapping check. Four groups retain nonzero unmodeled tail payload, three
leave a public definition uncovered, and two lack a usable retail contribution.

`BASE/Icondf2b` is the regression fixture. Its 37 candidate/retail `DIR32` sites, including two
public `uDimPal` anchors, prove a bijection for all thirteen four-byte function statics. The target
now emits their exact `_gFD*$S...` names and candidate offsets in a 52-byte BSS section, replacing
the former `const_001381xx` and unrelated string identities. Its focused score changes from
80.283134% to 81.06626% under `data_value` and to 81.03615% under `name_address`; the ordinary code
score remains 81.1265%.
