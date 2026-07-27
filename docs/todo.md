# TODO

- Move the strict-mode `SwapValues` template from `src/SOURCE/ARMYGRP.cpp` to an
  appropriate shared header, and audit the codebase for other same-type swaps
  where it can replace untyped temporaries without changing retail code generation.
