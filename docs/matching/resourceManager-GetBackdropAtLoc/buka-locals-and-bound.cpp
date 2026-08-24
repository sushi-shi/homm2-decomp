// resourceManager::GetBackdropAtLoc (PoL RVA 0xc8130; Buka RVA 0xb8040)
//
// Artifact:
//   build/source-variant-batch/get-backdrop-at-loc/results.json
// Complete 8 source arms x (clean + 50 forest/top states), 408/408. Seed
// 1213156658; source restored; wall-time and generator untruncated.
//
// `row -> curRow` and the Buka `destinationY + imageHeight` loop bound pair
// byte/relocation-identically with the PoL spellings in all 51 states. Adopt
// both. `dataWidth -> width` never closes: best 99.943665%; keep `dataWidth`.
// `/Od` frame audit: 1/1 aligned.
//
// The retained source has 30/51 exact state islands. Forest/top trial 2 is an
// audited 210-byte closure with exact 8/8 CFG and 9/9 ordered relocations.
// Clean live state remains 94.563380%, 212 bytes; hash-scoped MAX is 100.
