# Documentation map

The durable documentation is organized by purpose. Live queues, generated reports, and
one-run progress summaries belong under `build/`, not here.

This branch reconstructs Buka's stripped HMM2PL.exe (Gold 2.1 tree, VC6 SP5).
The PoL 2.0 line's binary-evidence and method documents (CodeView mine, VC4.2
toolchain contracts, the VC4.2 pattern catalog and matching matrices) were
deliberately not carried over: measured evidence does not port across compilers.
Their successors grow here from VC6-measured evidence.

## Binary and toolchain evidence

- `config/retail_functions.csv` (with its header notes) is the candidate function
  inventory; `config/delink_relocs.tsv` is the reviewed relocation-site manifest.
  Both are config-tracked because they are reviewed inputs, not generated output.
- `class-hierarchy.md`, `vendor-middleware.md`, and `strings-editor.md` retain
  reconstruction evidence that is expensive to rediscover.
- `resource-source-reconstruction-plan.md` records the deferred editable-resource work.
- `icon-format.md` and `icon-decoders.md` record the retail-verified icon (ICN) container,
  frame table, and both run-length pixel dialects, plus the per-function catalog for the
  `Icon*ToBitmap*` family. They also retain the confirmed retail clipping defect in the
  flipped decoders, whose dead branch must not be "simplified" away.
- `buka-annotation-worklist.tsv` maps the PoL-line reconstruction onto this image's
  addresses as an annotation work list.

## Data and final linking

- `candidate-data-topology.md`, `reviewed-data-objdiff.md`, `strict-data-allocations.md`,
  `coff-data-relocations.md`, and `data-symbol-normalization.md` define candidate/target
  object reconstruction.
- `relocation-canonicalization.md`, `static-storage-link-audit.md`,
  and `missing-public-data-audit.md` define the final-link checks. Object order for the
  link is the `config/units.toml` manifest order, audited against source anchors.

## Matching and proof

- `build-asserts.md` and `jump-tables.md` are reusable matching references.
- `patterns/` and `matching/` start empty on this branch and are founded by the first
  VC6-proven idioms and preserved structural attempts.
- `config/match_baseline.tsv` is the observation-only score ledger. It preserves a
  per-function maximum for the current normalized source hash, but has no ratchet or
  acceptance role.
- `homm2 audit historical-losses` compares every committed score-ledger epoch with the
  current live objdiff report. Its TSV separates unchanged effective-source hashes,
  body-to-dependency hash upgrades, changed hashes, and pre-hash evidence, and marks hash
  transitions whose immediately preceding maximum was exact. Write campaign output to `/tmp`
  rather than committing a queue snapshot.
- `constants-audit.md` defines the whole-tree numeric-literal inventory and per-file review
  contract.
- `iconf2bc-experiment-matrix.md` retains negative experiment evidence (measured on the
  PoL line's compiler; compiler-scoped).
- `archive/` contains contracts for retired experiment tools; it is not active workflow guidance.

## Retention rule

Keep a document when it is a tool contract, reproducible retail/toolchain evidence, an active
future plan, or negative experiment evidence that prevents repeated work. Remove it when it is
only a completed campaign report, duplicates an authoritative document, describes a live
queue/checkpoint as though it were permanent, or carries another compiler's measured evidence.
Source comments, versioned config, and generated reports remain authoritative for current
function and linker status.
