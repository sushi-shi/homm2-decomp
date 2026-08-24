// advManager::SetTownContext (PoL RVA 0x64101; Buka RVA 0xe186)
//
// Artifacts:
//   build/source-variant-batch/set-town-context-clean/results.json
//   build/source-variant-batch/set-town-context-names/results.json
// Clean screen: complete 2 pointer names x 2 index names x 2 page compares x
// 2 terrain compares, 16/16. Name census: complete 4 name arms x (clean + 50
// forest/top states), 204/204. Seed 1213156658; generator untruncated.
//
// Both comparison orientations share one text hash under every clean name
// shape. The PoL name pair is exact in all 51 states. `tp` alone is uniformly
// 99.934210%; `townNo` alone is 99.875000%; both Buka names are 99.907900%.
// Keep both PoL slot-sensitive names.
//
// PoL adopts Buka's context-first page and terrain comparisons. Final build:
// exact 535 bytes, exact 15/15 CFG, complete 25/25 ordered relocations.
