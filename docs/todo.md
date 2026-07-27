# TODO

- Move the strict-mode `SwapValues` template from `src/SOURCE/ARMYGRP.cpp` to an
  appropriate shared header, and audit the codebase for other same-type swaps
  where it can replace untyped temporaries without changing retail code generation.
- Evaluate a generic enum-indexed array wrapper that accepts a declared enum
  domain directly while preserving raw-array layout and retail code generation.
  Adopt it only as part of a codebase-wide indexing sweep; use explicit `IDX`
  conversions at individual array boundaries until then.
