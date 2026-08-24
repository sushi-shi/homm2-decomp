// advManager::StartCursor @ PoL RVA 0x0000d5e0
//
// PoL 2.0 source before:
//   if (walkSpeed > CONFIG_WALK_SPEED_SLOWEST)
//       m_cursorCycle = 1;
//   else
//       m_cursorCycle = SLOW_CURSOR_CYCLE_START;
//
// Buka 2.1 exact / retained PoL source:
//   m_cursorCycle = walkSpeed > CONFIG_WALK_SPEED_SLOWEST
//                       ? 1
//                       : SLOW_CURSOR_CYCLE_START;
//
// Complete matrix:
//   build/source-variant-batch/start-cursor-cycle/results.json
//   2 source arms * (clean + 50 forest/top states) = 102/102 cells
//   all 51 paired arms have equal text, ordered relocations, score, size,
//   and topology
//   clean: 99.975610%, 312 bytes, 6/6 blocks, 6/6 relocations
//   exact: trials 33 and 42, both source arms
//
// Audited retained-source replay:
//   build/tu-state-noise/start-cursor-cycle-exact-replay
//   seed 1213156658, forest/top, trial 33
//   100.000000%, 312 bytes, 6/6 blocks, ordered 6/6 relocations
//
// Disposition: invariant matching decomp; retain Buka ternary in PoL and
// bank the audited exact maximum. No gameplay change.

