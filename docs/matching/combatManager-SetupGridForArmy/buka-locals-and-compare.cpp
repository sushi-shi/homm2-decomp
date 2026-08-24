// combatManager::SetupGridForArmy (PoL RVA 0x340e; Buka RVA 0x38696)
//
// Artifacts:
//   build/source-variant-batch/setup-grid-army-hex-compare/results.json
//   build/source-variant-batch/setup-grid-local-census/results.json
//   build/source-variant-batch/setup-grid-old-side-full/results.json
//   build/source-variant-batch/setup-grid-old-index/results.json
//   build/source-variant-batch/setup-grid-buka-locals/results.json
//
// Hex comparison: complete 2 source arms x (clean + 50 forest/top states),
// 102/102. Both arms are exact in every paired state.
// Local census: complete 125/125 clean shapes. `hexIndex -> j` alone remains
// exact. `savedTargetSide -> oldSide` is uniformly 99.958900% over its
// complete 51-state census; `targetIndexSave -> oldIndex` is uniformly
// 99.972600%. The complete Buka three-name shape is uniformly 99.863014%
// over 51 states. Keep the two PoL slot-sensitive names.
//
// PoL adopts Buka's owner-first comparison and `j` loop index. Final source:
// exact 531 bytes, exact 23/23 CFG, complete 6/6 ordered relocations.
