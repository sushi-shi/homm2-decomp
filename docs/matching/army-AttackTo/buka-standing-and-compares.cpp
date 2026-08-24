// army::AttackTo(i32) (PoL RVA 0x4f756; Buka RVA 0x1d509)
//
// Artifacts:
//   build/source-variant-batch/army-attack-to-separated/results.json
//   build/source-variant-batch/army-attack-to-inline/results.json
// Seed 1213156658, forest/top. Both matrices complete; source restored.
//
// Separated-local matrix: 8 source arms x (clean + 50 states), 408/408.
// The Buka breath-target and first-step comparison orders are byte-neutral:
// all four positive-branch clean combinations are 100.000000%, 488 bytes,
// exact 25/25 CFG, 18 sites. PoL adopts both comparisons.
//
// De Morgan assignment arms: 99.488550%, 24/25 exact blocks in the clean
// state, independent of both comparison axes. Actual Buka inline ternary:
// complete 2 x 51 matrix; clean 99.305340%, alternate orbit 99.290080%,
// 24/25 exact blocks. Keep PoL's separate `finishStanding` local and positive
// branch; they are required 2.0 source shape, not portable Buka cleanup.
