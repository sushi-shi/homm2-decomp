/*
game::SetVisibility cross-version source families
=================================================

PoL 2.0 RVA 0x80b64; Buka 2.1 RVA 0x5b736.

Structural local/scope census: 102/102
--------------------------------------
  build/set-visibility-structural-state-manifest.json
  build/source-variant-batch/set-visibility-structural-state/results.json

The full PoL and Buka parents were compiled against clean plus the same 50
forest/top states. Clean: PoL 91.282760%, Buka 91.089650%, both 560 bytes.
Best shared state (trial 8): PoL 96.579310%, Buka 96.351720%, both 558 bytes.
All retained states have exact 37/37 CFG and ordered 19/19 relocations.

Disposition: retain PoL's VC4.2 slot family (`col`, `row`, `visibility`, and
block-local `distance`). Buka's `i`, `j`, `vis`, and hoisted `distance` are a
nonportable compiler/local-layout spelling, not a gameplay change.

Expression matrix: 408/408
--------------------------
  build/set-visibility-expression-state-manifest.json
  build/source-variant-batch/set-visibility-expression-state/results.json

Three independent axes cover distance-term order and the two mapExtra writes.
All eight source combinations are byte-identical within every clean/forest
state. The Buka donor arm reaches the same best trial-8 state: 96.579310%,
558 bytes, exact 37/37 CFG, and ordered 19/19 relocations. No exact island.

Disposition: invariant matching-decomp port of Buka's y-first distance sum
and both `*(mapExtra + col + MAP_WIDTH * row)` writes.
*/
