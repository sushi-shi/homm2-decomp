// searchArray::SeedCombatPosition @ PoL RVA 0x000a5200
//
// Correction (2026-08-24): there is no version boundary at the two tests.
// The earlier dossier trusted the reconstructed PoL `speed` member over the
// raw target. PoL retail reads army offsets 0xca at 0x4a52e7 and 0x4a5377;
// `tag_monsterInfo::shots` is at army+0xca, while `speed` is army+0xc5.
// Buka's exact source therefore supplies the missing member identity.
//
// Adapted Buka structural source:
//   seven dead 4-byte locals, enemy/index/hex live locals at function scope,
//   one shared index for all loops, and Buka's positive attack-mask condition.
//
// Clean structural product:
//   build/source-variant-batch/seed-combat-position-frame/results.json
//   PoL scoped locals:            97.601310%, 486/493 bytes
//   Buka frame + wrong speed member: 99.986930%, 493/493 bytes
//   Both have the exact 25/25 graph and 16/16 relocations.
//
// Complete structural/state product:
//   build/source-variant-batch/seed-combat-position-frame-state/results.json
//   2 arms * (clean + 50 forest/top states) = 102/102 cells
//   The adapted Buka frame is selected in the clean state and remains the
//   stable best 99.986930% retail-size island.
//
// Complete condition/state product:
//   build/source-variant-batch/seed-combat-position-condition-state/results.json
//   4 arms * (clean + 50 forest/top states) = 204/204 cells
//   Positive versus nested PoL-equivalent condition source is byte- and
//   relocation-neutral at both sites in every paired state. Exact Buka source
//   therefore decides the retained positive spelling. It did not test the
//   member identity; the two remaining target bytes were the 0xc5/0xca
//   displacements.
//
// Unchanged-source clue artifacts:
//   build/seed-combat-position-selected-state-summary.json
//   build/tu-state-noise/seed-combat-position-selected
//   50/50 forest/top states, four byte/relocation states; the clean baseline is
//   best at 99.986930%, retail size, exact CFG, ordered 16/16 relocations, and
//   two residual bytes before the member correction.
//
// Final retained source: both tests use `m_monster.shots`. Normal `homm2 build`
// closes at 100.000000%, 493 bytes, exact 25/25 CFG, and ordered 16/16
// relocations. This is a source-correction port with no retail behavior change.
