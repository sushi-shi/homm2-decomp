// advManager::IsCrystalBallInEffect (PoL RVA 0x6c241; Buka RVA 0x158e9)
//
// Artifacts:
//   build/source-variant-batch/crystal-ball-clean/results.json
//   build/source-variant-batch/crystal-ball-shape/results.json
//   build/source-variant-batch/crystal-ball-structural/results.json
//   build/source-variant-batch/crystal-ball-name/results.json
// Complete 16-arm and 8-arm clean screens; structural and name censuses each
// complete 4 arms x (clean + 50 forest/top states), 204/204. Seed 1213156658;
// source restored; wall-time and generators untruncated.
//
// Buka's X-first squared-distance sum pairs byte/relocation-identically with
// the PoL sum in all 51 states; adopt it. `heroIndex -> i` never closes (clean
// 98.692310%, best 99.907690%); `range -> distance` never closes (clean
// 98.600000%, best 99.815384%). Keep both PoL slot-sensitive names.
//
// Direct `range <= radius` / `radius >= range` spellings pair in all states.
// Guard/continue and empty-then/else forms are 220 bytes at 97.230770%; the
// guarded if/else is 225 bytes at 95.692310%. Keep the compact direct return.
// `/Od` frame audit: 1/1 aligned.
//
// The retained source has 9/51 exact state islands. Forest/top trial 8 is an
// audited 215-byte closure with exact 9/9 CFG and 6/6 ordered relocations.
// Clean live state is 98.769230%; hash-scoped MAX is 100.
