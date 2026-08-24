// searchArray::TestPossibleDirections @ PoL RVA 0x000a4e90
//
// Baseline PoL source:
//   do/while loop, one invalidDirection block, raw object-type masks, indexed
//   mapExtra read, and several compound object/trigger conditions
//   92.800830%, 880/879 bytes, 67/66 relocations
//
// Buka structural source:
//   for loop, explicit TERRAIN_INVALID stores at every rejected direction,
//   split visibility/object guards, m_objectTileset accessors, direct flat
//   mapExtra read, and the positive StopOnTrigger ownership
//
// Clean structural product:
//   build/source-variant-batch/test-directions-structure/results.json
//   PoL shared-invalid family: 92.800830%, 880 bytes
//   Buka longhand family:      98.568470%, 881 bytes, exact 51/51 graph
//   Both have 67/66 relocations in the clean state.
//
// Complete structural/state product:
//   build/source-variant-batch/test-directions-structure-state/results.json
//   2 arms * (clean + 50 forest/top states) = 102/102 cells
//   selected Buka best is forest trial 8: 99.273860%, 876/879 bytes,
//   exact 51/51 graph, 66/66 relocation count; ordered relocation identity
//   and three bytes still prevent closure.
//
// Complete internal spelling/state product:
//   build/source-variant-batch/test-directions-internal-state/results.json
//   8 arms * (clean + 50 forest/top states) = 408/408 cells
//   Bounds ownership, flat/indexed mapExtra access, and tileset/raw-mask object
//   tests are byte- and relocation-neutral in every paired state. Retained PoL
//   source uses the authoritative Buka spellings.
//
// Unchanged-source clue artifacts:
//   build/test-directions-selected-state-summary.json
//   build/tu-state-noise/test-directions-selected
//   50/50 forest/top states, seven byte/relocation states; best fuzzy/topology
//   trial 8 as above. Best structural frontier is trial 22 at 99.211620%,
//   884 bytes, exact graph, and one size-only block. No exact island.
//
// The direct --blocks diagnostic hangs on this function; the complete public
// products and retained direct-census artifacts carry the canonical CFG data.
//
// Disposition: matching decomp; retain the full Buka control-flow and accessor
// family in PoL. No gameplay change. The residual is a compiler-state/ordered-
// relocation island, not evidence for restoring the old shared-invalid CFG.
