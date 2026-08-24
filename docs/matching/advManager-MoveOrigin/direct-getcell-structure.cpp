// advManager::MoveOrigin @ PoL RVA 0x0000ff07
//
// Baseline PoL source:
//   four mapCell* locals, three raw cells + width*y + x expressions,
//   one GetCell call, and four DebugCheck() hooks
//   99.358025%, 592/587 bytes, 11/10 blocks, 0/0 relocations
//
// Buka structural product:
//   build/source-variant-batch/move-origin-structure/results.json
//
//   arm                              score       size  blocks
//   PoL pointer locals               99.358025    592   11/10
//   direct GetCell, PoL order        99.975310    587   10/10 exact graph
//   direct GetCell, origin first     99.962960    587   10/10 exact graph
//   full Buka names/order            99.962960    587   10/10 exact graph
//
// Complete structural/state product:
//   build/source-variant-batch/move-origin-structure-state/results.json
//   4 arms * (clean + 50 forest/top states) = 204/204 cells
//   selected direct/PoL-order family: best 99.987656% at trials 4, 5, 7,
//   17, and 23; retail size, exact graph, 0/0 relocations
//   origin-first/full-Buka family: best 99.975310%
//
// Name product:
//   build/source-variant-batch/move-origin-names/results.json
//   PoL and Buka local names/declaration order are identical in all 51 paired
//   states, so retained source uses Buka's oldOriginX/oldOriginY/cellX/cellY.
//
// First-divergence product:
//   build/source-variant-batch/move-origin-first-y/results.json
//   102/102 cells; flipping only the first post-scroll cellY sum is erased in
//   every pair and does not close the four-byte operand island.
//
// Unchanged-source clue artifacts:
//   build/tu-state-noise/move-origin-selected
//   build/tu-state-noise/move-origin-layer2
//   Layer 1: four states; best trial 4 is four bytes from retail.
//   Layer 2: no closure; two distinct four-byte states remain.
//
// Retained source consequence: the structural change moves SOURCE/CURSOR TU
// state. UnwindMapChangeQueue drops from live exact to 99.409836%; TurnTo,
// ProcessIncomingSingleMapChange, ProcessIncomingGroupMapChange, and
// SendMapChange change raw state without losing exact scores. Recover each
// sibling independently.
//
// Disposition: matching decomp; retain Buka direct-GetCell structure and
// names with PoL cursor-first operand order. No gameplay change. Remaining
// MoveOrigin residual is compiler state, not a source steering request.

