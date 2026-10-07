# Project documentation

- [Matching workflow](tooling.md), [build gates](build-asserts.md),
  [the scenario editor image](editor.md),
  [command map](tooling-map.md), [repository workflow](workflow.md),
  [tooling convergence](tooling-convergence.md).
- [Version lineage](versions/README.md): [Gold 2.1 Buka changes](versions/gold-2.1-buka.md),
  [cross-version spellings](versions/cross-version-spellings.md),
  [slot layout](versions/cross-version-slot-layout.md),
  [base selection](versions/base-selection-audit.md).
- [Other builds](builds.md), [retail-exact linking](retail-exact-link.md),
  [playing the build](play.md), [generated source branches](clean-source.md),
  [localization](localization.md) and its
  [English provenance](localization-english-provenance.md).
- [Score tracking](match-status.md), [match provenance](match-provenance-audit.md),
  [compiler patterns](patterns/INDEX.md),
  [compiler allocation order](compiler-re-allocation-order.md),
  [compiler-generated functions](compiler-generated-functions.md),
  [jump tables](jump-tables.md).
- [Constants](constants-audit.md), [enum and constant reuse](enum-reuse.md),
  [strict-enum Clang build](clang-strict-enum-build-errors.md),
  [semantic naming](semantic-naming-audit.md),
  [readability](readability/README.md).
- [Relocation manifest](reloc-manifest-sweep.md),
  [class hierarchy](class-hierarchy.md), [vendor middleware](vendor-middleware.md),
  [icon format](icon-format.md) and [decoders](icon-decoders.md),
  [resource plan](resource-source-reconstruction-plan.md).
- Data and linking: [candidate data topology](candidate-data-topology.md),
  [reviewed data](reviewed-data-objdiff.md),
  [strict allocations](strict-data-allocations.md),
  [COFF data relocations](coff-data-relocations.md),
  [data symbol normalization](data-symbol-normalization.md),
  [relocation canonicalization](relocation-canonicalization.md),
  [static storage](static-storage-link-audit.md),
  [missing public data](missing-public-data-audit.md),
  [linked data-section walls](linked-data-section-walls.md),
  [linked function-placement walls](linked-function-placement-walls.md).

Retail facts live in `config/retail`, build contracts in `config`, generated
state in `build`, and reusable compiler mechanisms in `docs/patterns`. The PoL
2.0 line's VC4.2 evidence was not carried over: measured evidence does not port
across compilers. Keep a document when it is a tool contract, reproducible
retail or toolchain evidence, an active plan, or negative evidence that prevents
repeated work. Campaign logs, attempt dossiers and worklists stay out of the
tree; the matching and reconstruction dossiers removed from `docs/` remain at
`f0ae961d2`.
