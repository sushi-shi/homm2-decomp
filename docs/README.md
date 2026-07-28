# Documentation map

The durable documentation is organized by purpose. Live queues, generated reports, and
one-run progress summaries belong under `build/`, not here.

## Binary and toolchain evidence

- `codeview-contents.md`, `compiler-detection.md`, and `toolchain-vc42.md` record what the
  retail executable proves and how to reproduce the compiler/linker environment.
- `class-hierarchy.md`, `vendor-middleware.md`, `vc42-libcmt-gf.md`, and the two `strings-*.md`
  catalogs retain reconstruction evidence that is expensive to rediscover.
- `resource-link-reconstruction.md` documents the active resource link; the explicitly
  deferred `resource-source-reconstruction-plan.md` records the later editable-resource work.
- `icon-format.md` and `icon-decoders.md` record the retail-verified icon (ICN) container,
  frame table, and both run-length pixel dialects, plus the per-function catalog for the
  `Icon*ToBitmap*` family. They also retain the confirmed retail clipping defect in the
  flipped decoders, whose dead branch must not be "simplified" away.

## Data and final linking

- `candidate-data-topology.md`, `reviewed-data-objdiff.md`, `strict-data-allocations.md`,
  `coff-data-relocations.md`, `data-symbol-normalization.md`, and
  `delinker-contribution-manifest.md` define candidate/target object reconstruction.
- `relocation-canonicalization.md`, `static-storage-link-audit.md`,
  and `missing-public-data-audit.md` define the final-link checks.
- `rdata-contribution-audit.md` records the resolved readonly-layout investigation and retains
  older contribution measurements as explicitly labeled historical evidence; current offsets
  come from the link report.

## Matching and proof

- `build-asserts.md`, `od-stack-layout.md`, `jump-tables.md`, and `patterns/` are reusable
  matching references.
- `msvc42-tu-state-noise-search.md` defines the controlled disposable compiler-state probe
  and its exact-only contribution to retained maximum metrics.
- `config/match_baseline.tsv` is the observation-only score ledger. It preserves a
  per-function maximum for the current normalized source hash, but has no ratchet or
  acceptance role.
- `scripts/historical_exact_losses.py` compares every committed score-ledger epoch with the
  current live objdiff report. Its TSV separates unchanged effective-source hashes,
  body-to-dependency hash upgrades, changed hashes, and pre-hash evidence, and marks hash
  transitions whose immediately preceding maximum was exact. Write campaign output to `/tmp`
  rather than committing a queue snapshot.
- `constants-audit.md` defines the whole-tree numeric-literal inventory and per-file review
  contract.
- `iconf2bc-experiment-matrix.md` and `matching-matrices/` retain negative experiment evidence.
- `archive/` contains contracts for retired experiment tools; it is not active workflow guidance.

## Retention rule

Keep a document when it is a tool contract, reproducible retail/toolchain evidence, an active
future plan, or negative experiment evidence that prevents repeated work. Remove it when it is
only a completed campaign report, duplicates an authoritative document, or describes a live
queue/checkpoint as though it were permanent. Source comments, versioned config, and generated
reports remain authoritative for current function and linker status.
