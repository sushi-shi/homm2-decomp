// game::ExperienceValueOfStack (PoL RVA 0x80f68; Buka RVA 0x5bad4)
//
// Axis: hit-points-first / quantity-first multiplication.
// Artifact:
//   build/source-variant-batch/experience-stack-product/results.json
// Coverage: complete 2 source arms x (clean + 50 forest/top states), 102/102.
// Seed: 1213156658. Source restored.
//
// Both clean arms are exact: 145 bytes, exact 10/10 CFG, one relocation.
// Every paired state gives both arms the same score, size, topology, and
// relocation hash. PoL adopts the authoritative Buka quantity-first source.
