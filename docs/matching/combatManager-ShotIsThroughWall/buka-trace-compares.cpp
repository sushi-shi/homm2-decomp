// combatManager::ShotIsThroughWall (PoL RVA 0x95559; Buka RVA 0x2aabf)
//
// Axes: wall table/trace order; tower table/trace order.
// Artifact:
//   build/source-variant-batch/shot-through-wall-trace-compare/results.json
// Coverage: complete 4/4 clean-source Cartesian product; source restored.
//
// Every arm emits the same 99.910200% probe object: 838 bytes, exact 44/44
// CFG, and 25/25 relocation count. PoL adopts both exact-Buka trace-first
// comparisons. The existing local-slot residual is shared and unrelated.
// No gameplay or emitted-byte change within the measured state.
//
// Final unchanged-source state census:
//   build/shot-through-wall-final-state-summary.json
//   build/shot-through-wall-final-replay-summary.json
// Seed 1213156658, forest/top, trial 1 reproduced exact closure: 838 bytes,
// exact 44/44 CFG, zero retail-byte delta, and the complete ordered 25/25
// relocation sequence. The replay audited and retained MAX 100.0000 for
// source hash 3c81127dad29; generated declarations were not retained.
