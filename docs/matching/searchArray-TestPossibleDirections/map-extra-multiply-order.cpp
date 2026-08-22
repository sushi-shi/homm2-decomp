// searchArray::TestPossibleDirections, RVA 0x4a136
//
// Reviewed axis:
//   row_first:   MAP_EXTRA_AT(gSearchNextX, gSearchNextY)
//   width_first: MAP_EXTRA_AT_WFIRST(gSearchNextX, gSearchNextY)
//
// Artifacts:
//   build/test-possible-directions-map-extra-axes.json
//   build/test-possible-directions-map-extra-manifest.json
//   build/source-variant-batch/test-possible-directions-map-extra/results.json
//
// The matrix completed all 102 candidates: two source arms across the clean
// state and 50 declaration-forest TU states. It was not wall-time truncated,
// and the permuter restored the source. All 51 width_first candidates were
// exact; all 51 row_first candidates remained non-exact. The clean
// width_first arm matched 1369/1369 bytes, 98/98 relocations, and all 80 CFG
// blocks, with no exact-rejection reason. Disposition: RETAIN width_first.

if (gbHumanPlayer[giCurPlayer] != 0
    && (MAP_EXTRA_AT_WFIRST(gSearchNextX, gSearchNextY) & giCurPlayerBit) == 0) {
    gSearchTerrain = TERRAIN_INVALID;
    goto storeDirection;
}
