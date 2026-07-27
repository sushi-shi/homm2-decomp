/*
 * IconToBitmapScale reviewed structural campaign, RVA 0xd2f90.
 *
 * Initial canonical source:
 *   live 73.482300%; retained MAX 85.631200%; size 361 versus retail 377;
 *   4/4 relocations; 16 candidate blocks versus 18 retail blocks. The first
 *   branch-kind divergence was at the inner-loop entry.
 *
 * 1. Guarded loop-counter initialization:
 *    build/tu-state-noise/icon2bs-guarded-counter-init-20260727
 *    Complete clean + 50-state forest census.
 *
 *      i32 rows;
 *      if (0 < scale) {
 *          rows = scale;
 *          do {
 *              ...
 *              i32 cols;
 *              if (0 < scale) {
 *                  cols = scale;
 *                  do {
 *                      ...
 *                  } while (cols != 0);
 *              }
 *              ...
 *          } while (rows != 0);
 *      }
 *
 *    This is the retail-compatible CFG family. Moving each counter
 *    materialization behind its positive-scale guard restores the two missing
 *    blocks: 18/18 flow-exact, with 13 exact-size and five size-only blocks.
 *    Clean source reaches 75.695040%, size 361. Forest trials 3, 15, 30, and
 *    43 reach 85.829790%; trial 3 is size 365. All four external relocation
 *    identities, order, and addends agree with retail.
 *
 *    The source shape is retained. Its trial-3 MAX was reproduced and recorded
 *    only through the generated search tools:
 *    build/tu-state-noise/icon2bs-guarded-counter-trial3-record-max-20260727
 *    source hash 95a045323ecc; recorded MAX 85.8298%.
 *
 * 2. Natural scale-value alias:
 *    build/tu-state-noise/icon2bs-scaled-size-alias-20260727
 *    Complete clean + 50-state forest census.
 *
 *      i32 scaledSize = scale;
 *      if (scaledSize == SCALE_NATIVE_SIZE) { ... }
 *      i32 step = SCALE_NATIVE_SIZE / scaledSize;
 *      ...
 *
 *    The optimized alias collapses to the same principal compiler states and
 *    the same 85.829790% ceiling. It adds no semantic information and is not
 *    retained. The direct parameter spelling was restored.
 *
 * 3. Surviving-shape expression matrix:
 *    build/match-variants/icon2bs-surviving-shape-20260727
 *    Complete product: 3 * 2 * 2 * (clean + 50 forest states) = 612.
 *
 *    Clear-buffer ownership arms:
 *      i32 rowOff = 0;
 *      u8* tmpPixels = tmp->m_pixels;
 *      memset(tmpPixels + rowOff, 0, SCALE_NATIVE_SIZE);
 *
 *      u8* tmpPixels = tmp->m_pixels;
 *      i32 rowOff = 0;
 *      memset(tmpPixels + rowOff, 0, SCALE_NATIVE_SIZE);
 *
 *      i32 rowOff = 0;
 *      u8* tmpPixels = tmp->m_pixels;
 *      memset(&tmpPixels[rowOff], 0, SCALE_NATIVE_SIZE);
 *
 *    Both inner and outer loop tails crossed explicit assignments with their
 *    compound equivalents:
 *      dstPix = dstPix + 1;  / dstPix++;
 *      srcPix = srcPix + step; / srcPix += step;
 *      cols = cols - 1; / cols--;
 *      srcRow = srcRow + srcAdv; / srcRow += srcAdv;
 *      dstRow = dstRow + pitch; / dstRow += pitch;
 *      rows = rows - 1; / rows--;
 *
 *    Indexed versus additive clear addresses and every tail spelling are
 *    byte-neutral within each state. Swapping tmpPixels before rowOff enters a
 *    coherent lower island whose best is 83.340420%, size 365. The unchanged
 *    declaration order remains best at 85.829790%.
 *
 * Disposition: retain only the guarded counter lifetimes. The remaining five
 * size-only blocks are optimized register/spill ownership in setup, clear-row,
 * inner-loop entry, and outer-loop tail; no tested expression spelling repairs
 * them or changes the complete ordered relocation semantics.
 */

void IconToBitmapScale_guarded_loop_structure_attempts() {
}
