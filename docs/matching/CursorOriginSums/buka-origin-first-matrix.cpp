// SOURCE/CURSOR cursor/previous-position + map-origin operand order
//
// Buka 2.1 exact source puts the origin term first. Five complete VC4.2
// products test every simple differing sum (18 expressions total):
//
//   build/source-variant-batch/cursor-origin-start-cursor/results.json
//   build/source-variant-batch/cursor-origin-stop-cursor/results.json
//   build/source-variant-batch/cursor-origin-move-hero/results.json
//   build/source-variant-batch/cursor-origin-valid-move/results.json
//   build/source-variant-batch/cursor-origin-move-origin/results.json
//
// Each product is 2 source arms * (clean + 50 forest/top states) = 102/102.
//
//   function     clean cursor/origin   best cursor/origin    disposition
//   StartCursor   99.975610/99.951220  100.000000/99.975610 keep PoL
//   StopCursor    99.970146/99.970146  100.000000/100.000000 port Buka
//   MoveHero      97.095955/97.094010   97.301025/97.299070 keep; retest
//   ValidMove     99.988335/99.982506   99.994170/99.988335 keep PoL
//   MoveOrigin    99.358025/99.345680   99.370370/99.358025 keep; retest
//
// StopCursor is byte/relocation/topology-identical in all 51 pairs. The
// other four select cursor-first in every state. MoveHero and MoveOrigin have
// larger open structural parents, so their negative result is parent-local.
//
// StopCursor reset product:
//   build/source-variant-batch/stop-cursor-family/results.json
//   2 origin arms * 2 reset arms * 51 states = 204/204 cells
//   chained reset: 284 bytes, best 100.000000%
//   Buka repeated literals: 279 bytes, best 95.074620%
//
// Retained-source exact replay:
//   build/tu-state-noise/stop-cursor-origin-exact
//   seed 1213156658, forest/top, trial 22
//   100.000000%, 284 bytes, 7/7 blocks, ordered 5/5 relocations
//
// Disposition: retain Buka origin-first only in StopCursor. Keep PoL's
// chained reset, StartCursor, and ValidMove spellings. Revisit MoveHero and
// MoveOrigin origin order only after their structural-parent matrices.

