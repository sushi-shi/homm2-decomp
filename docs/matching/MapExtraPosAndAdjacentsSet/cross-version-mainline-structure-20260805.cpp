/*
 * Cross-version structural matrix for MapExtraPosAndAdjacentsSet.
 *
 * Target: SOURCE/ADVMGR, PoL RVA 0x0006a0e7.
 * Source donor: the exact decomp-gold-2.1-buka body at VA 0x00413b4d.
 * Matrix: build/mapextra-cross-version-manifest.json
 * Artifacts: build/source-variant-batch/mapextra-cross-version and
 *            build/tu-state-noise/mapextra-cross-version-exact
 *
 * Arms:
 *   1. the previous PoL reconstruction;
 *   2. the exact Buka/mainline pointer reads and guard-clause loops;
 *   3. the mainline pointer reads inside the previous PoL CFG.
 *
 * The initial run found the exact mainline arm at forest/top trial 14,
 * seed 1213156658. A focused replay after retaining that arm proved retail
 * size 246, zero byte delta, exact 20/20 CFG topology, and the complete 6/6
 * ordered relocation stream. The generated declaration forest was removed.
 * The exact observation is banked for source hash 715ac9bad32c and harvested
 * under docs/matching-matrices/max-asm/.
 *
 * Disposition: retained. This is a shared-source invariant: the exact VC6
 * structure also reaches exact PoL bytes under VC4.2.
 */

i32 MapExtraPosAndAdjacentsSet(i32 x, i32 y, u8 mask) {
    if (*(mapExtra + x + MAP_WIDTH * y) & mask) {
        return 1;
    }
    for (i32 checkX = x - 1; checkX <= x + 1; ++checkX) {
        if (checkX < 0 || checkX >= MAP_WIDTH) {
            continue;
        }
        for (i32 checkY = y - 1; checkY <= y + 1; ++checkY) {
            if (checkY < 0 || checkY >= MAP_HEIGHT) {
                continue;
            }
            if (*(mapExtra + checkX + MAP_WIDTH * checkY) & mask) {
                return 1;
            }
        }
    }
    return 0;
}
