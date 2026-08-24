// searchArray::SeedCombatPosition @ PoL RVA 0x000a5200
//
// Version boundary preserved:
//   PoL tests unit->m_monster.speed; Buka retail tests shots. The Buka shots
//   rule is a documented gameplay change and is not imported.
//
// Adapted Buka structural source:
//   seven dead 4-byte locals, enemy/index/hex live locals at function scope,
//   one shared index for all loops, and Buka's positive attack-mask condition,
//   with both shots reads changed back to PoL speed
//
// Clean structural product:
//   build/source-variant-batch/seed-combat-position-frame/results.json
//   PoL scoped locals:            97.601310%, 486/493 bytes
//   Buka frame + PoL speed logic: 99.986930%, 493/493 bytes
//   Both have the exact 25/25 graph and 16/16 relocations.
//
// Complete structural/state product:
//   build/source-variant-batch/seed-combat-position-frame-state/results.json
//   2 arms * (clean + 50 forest/top states) = 102/102 cells
//   The adapted Buka frame is selected in the clean state and remains the
//   stable best 99.986930% retail-size island.
//
// Complete speed-condition/state product:
//   build/source-variant-batch/seed-combat-position-condition-state/results.json
//   4 arms * (clean + 50 forest/top states) = 204/204 cells
//   Positive versus nested PoL-equivalent condition source is byte- and
//   relocation-neutral at both sites in every paired state. Exact Buka source
//   therefore decides the retained positive spelling, still using speed.
//
// Unchanged-source clue artifacts:
//   build/seed-combat-position-selected-state-summary.json
//   build/tu-state-noise/seed-combat-position-selected
//   50/50 forest/top states, four byte/relocation states; the clean baseline is
//   best at 99.986930%, retail size, exact CFG, ordered 16/16 relocations, and
//   two residual bytes. No closure.
//
// Disposition: matching decomp frame/loop port with PoL gameplay logic. Keep
// Buka's shots-based behavior confined to the version-change documentation.
