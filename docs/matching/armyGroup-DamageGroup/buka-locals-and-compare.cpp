// armyGroup::DamageGroup (PoL RVA 0x8c7d2; Buka RVA 0x21d87)
//
// Artifacts:
//   build/source-variant-batch/damage-group-kill-compare/results.json
//   build/source-variant-batch/damage-group-local-names-complete/results.json
// Seed 1213156658, forest/top. Source restored.
//
// Kill-count comparison: complete 2 x 51 matrix, 102/102. Both arms pair in
// every state; clean arms are exact at 333 bytes, 22/22 CFG, five relocations.
// PoL adopts Buka's killed-first comparison.
//
// Local names: complete, untruncated 16 x 51 product, 816/816. Buka's
// `killChance/killed` pair is 99.709305% in every state; killChance alone is
// 99.767440% and killed alone 99.709305%. Keep exact PoL names
// percentChance/numKilled; the Buka renames are compiler-specific.
