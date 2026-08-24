// CreateJoinFile (PoL RVA 0x85107; Buka RVA 0x5f9cd)
//
// Axis: `diffSize1 > position1` / `position1 < diffSize1`.
// Artifact:
//   build/source-variant-batch/create-join-file-diff-loop/results.json
// Coverage: complete 2 source arms x (clean + 50 forest/top states), 102/102.
// Seed: 1213156658. Source restored.
//
// Both clean arms emit the same retail code bytes: 974 bytes, exact 24/24
// CFG, and 77 relocation sites. All paired state arms also share code and
// relocation hashes. The batch exact gate reports 19 private `$SG` versus
// source `DATA_COMPGEN` identity spellings; raw sites, types, and addends are
// identical and `homm2 relocs` resolves the canonical model with 77/77 and
// only-base=0. PoL adopts the authoritative Buka position-first spelling.
