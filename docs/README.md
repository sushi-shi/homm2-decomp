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

## Data and final linking

- `candidate-data-topology.md`, `reviewed-data-objdiff.md`, `strict-data-allocations.md`,
  `coff-data-relocations.md`, `data-symbol-normalization.md`, and
  `delinker-contribution-manifest.md` define candidate/target object reconstruction.
- `relocation-canonicalization.md`, `static-storage-link-audit.md`,
  `missing-public-data-audit.md`, and `link-layout-tracker.md` define the final-link checks.
- `rdata-contribution-audit.md` records the resolved readonly-layout investigation and retains
  older contribution measurements as explicitly labeled historical evidence; current offsets
  come from `homm2 link-layout` and the link report.

## Matching and proof

- `build-asserts.md`, `od-stack-layout.md`, `jump-tables.md`, and `patterns/` are reusable
  matching references.
- `breadth-audit-tracker.md` documents the active machine-readable audit state in
  `config/breadth_audit.json`.
- `base-wall-ledger.md`, `matching-soft-defer-ledger.md`, `iconf2bc-experiment-matrix.md`, and
  `matching-matrices/` are no-repeat evidence. Keep them while their affected functions remain
  non-exact; do not turn them into general instructions or live assignment queues.

## Retention rule

Keep a document when it is a tool contract, reproducible retail/toolchain evidence, an active
future plan, or negative experiment evidence that prevents repeated work. Remove it when it is
only a completed campaign report, duplicates an authoritative document, or describes a live
queue/checkpoint as though it were permanent. Source comments, versioned config, and generated
reports remain authoritative for current function and linker status.
