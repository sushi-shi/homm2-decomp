// playerData::NextHero (PoL RVA 0x70d1a; Buka RVA 0x4c457)
//
// Axes: current/curHero local-name family; array-first/current-first compare.
// Artifact: build/source-variant-batch/next-hero-buka/results.json
// Coverage: complete 4/4 clean-source Cartesian product; source restored.
//
// Both `current` arms are exact at 301 bytes, 21/21 CFG, and ordered 11/11
// relocations. Both `curHero` arms are 99.756410% with the same size/CFG/
// count. PoL adopts Buka's current-hero-first comparison and keeps its exact
// compiler-specific `current` slot name. No gameplay change.
