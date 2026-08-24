// combatManager::ResetBoltAngle (SOURCE/SPELLS, RVA 0x251cd)
//
// Baseline census:
//   build/reset-bolt-angle-cross-version-state-summary.json
//   build/tu-state-noise/reset-bolt-angle-cross-version
// Clean source is 99.982980%, 800 bytes, exact 38/38 CFG and ordered 30/30
// relocations. Forest/top trial 3 is an audited exact island.
//
// Independent arithmetic product (64/64, complete):
//   build/reset-bolt-angle-axes.json
//   build/source-variant-batch/reset-bolt-angle
// Axes: distance comparison ownership, explicit float divisor, widthLast add
// order, average-angle scale order/cast, angle-distance comparison ownership,
// and random-angle scale order/cast. All 64 arms produce one VC4.2 text and
// relocation state.
//
// Selected-family state product (102/102, complete):
//   build/reset-bolt-angle-package-axes.json
//   build/source-variant-batch/reset-bolt-angle-package
// Current PoL and full Buka arithmetic families are paired over clean plus 50
// forest/top states. Every one of the 51 pairs has identical text hash, size
// and normalized relocation hash. The Buka arithmetic family is retained.
//
// Local-name state product (102/102, complete):
//   build/reset-bolt-angle-name-axes.json
//   build/source-variant-batch/reset-bolt-angle-names
// Buka's VC6 name set differs in all 51 paired states and lowers the best batch
// score from 99.893616% to 99.812770%. Keep the PoL VC4.2 slot names.
//
// Retained-source exact replay:
//   build/reset-bolt-angle-retained-trial3-summary.json
//   build/tu-state-noise/reset-bolt-angle-retained-trial3
// Forest/top seed 1213156658 trial 3 is byte-exact at retail size with exact
// CFG and the complete ordered relocation stream. MAX 100 is recorded for
// source hash 35f269802638; generated declarations are not retained.
//
// Disposition: invariant matching-decomp transfer; no PoL behavior change.
