# Project documentation

- [Matching workflow](tooling.md), [build gates](build-asserts.md),
  [the scenario editor image](editor.md),
  [command map](tooling-map.md), [repository workflow](workflow.md),
  [tooling convergence](tooling-convergence.md).
- [Version changes](version-changes.md) (PoL 2.0 → Gold 2.1 → Buka),
  [cross-version spellings](cross-version-spellings.md),
  [base selection](base-selection-audit.md).
- [Other builds](builds.md), [retail-exact linking](retail-exact-link.md),
  [playing the build](play.md), [generated source branches](clean-source.md),
  [localization](localization.md).
- [Score tracking](match-status.md), [compiler patterns](patterns/INDEX.md),
  [matching attempts](matching/), [jump tables](jump-tables.md),
  [constants](constants-audit.md), [enum and constant reuse](enum-reuse.md), a
  [negative experiment matrix](iconf2bc-experiment-matrix.md).
- [Relocation manifest](reloc-manifest-sweep.md),
  [class hierarchy](class-hierarchy.md), [vendor middleware](vendor-middleware.md),
  [editor strings](strings-editor.md), [icon format](icon-format.md) and
  [decoders](icon-decoders.md), [resource plan](resource-source-reconstruction-plan.md).
- Data and linking: [candidate data topology](candidate-data-topology.md),
  [reviewed data](reviewed-data-objdiff.md),
  [strict allocations](strict-data-allocations.md),
  [COFF data relocations](coff-data-relocations.md),
  [data symbol normalization](data-symbol-normalization.md),
  [relocation canonicalization](relocation-canonicalization.md),
  [static storage](static-storage-link-audit.md),
  [missing public data](missing-public-data-audit.md).

Retail facts live in `config/retail`, build contracts in `config`, generated
state in `build`, and reusable compiler mechanisms in `docs/patterns`. The PoL
2.0 line's VC4.2 evidence was not carried over: measured evidence does not port
across compilers. Keep a document when it is a tool contract, reproducible
retail or toolchain evidence, an active plan, or negative evidence that prevents
repeated work; `archive/` holds contracts of retired tools.
