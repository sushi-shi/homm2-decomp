// searchArray::PushCombatPoint @ PoL RVA 0x000a5800
//
// PoL source before:
//   nested ValidHex/speed/visited/capacity wrappers, low-before-high locals,
//   raw m_storage.nodes access, low+high midpoint, maxQueueCount-first compare
// Buka exact / retained PoL source:
//   four early returns, high-before-low function-scope locals, GetNode access,
//   high+low midpoint, queueCount-first max comparison
//
// Clean structural product:
//   build/source-variant-batch/push-combat-point-structure/results.json
//   PoL nested family:       91.691490%, 251/256 bytes, 2/2 relocations
//   Buka early-return family: 94.138300%, 251/256 bytes, 2/2 relocations
//   Buka has the better 16/17 block rank; one target-tail block remains.
//
// Complete structural/state product:
//   build/source-variant-batch/push-combat-point-structure-state/results.json
//   2 arms * (clean + 50 forest/top states) = 102/102 cells
//   Buka is selected; forest trial 38 reaches 98.191490%, 251 bytes,
//   16/17 blocks, and ordered 2/2 relocations.
//
// Unchanged-source clue artifacts:
//   build/push-combat-point-selected-state-summary.json
//   build/tu-state-noise/push-combat-point-selected
//   50/50 forest/top states, seven byte/relocation states; trial 38 is seven
//   raw bytes from retail. The sole missing five-byte target-tail block also
//   appears after retail's ret in the provisional disassembly boundary.
//
// Disposition: matching decomp; retain the full Buka early-return/search-local
// family in PoL. No gameplay change. Do not call the target closed while the
// provisional tail block and seven instruction bytes remain.
