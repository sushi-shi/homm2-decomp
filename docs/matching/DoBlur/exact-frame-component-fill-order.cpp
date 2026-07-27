/*
 * DoBlur exact-frame matrix: component-table fill assignment order.
 *
 * All six semantic permutations of these assignments were crossed with the
 * clean state and 50 target-adjacent TU states:
 *
 *     redComponents[offset]   = paletteColor[-3];
 *     greenComponents[offset] = paletteColor[-2];
 *     blueComponents[offset]  = paletteColor[-1];
 *
 * Result (2026-07-26):
 *   complete matrix: 6 x 51 = 306/306
 *   trial-40 scores: 93.696205% through 93.708860%
 *   size: 1705 in every trial-40 arm
 *   ordered relocation count: 43/43
 *
 * All six trial-40 target byte streams were retained in results.json and
 * compared directly.  Relative to red/green/blue, every alternative differs
 * only at target offsets 0xc9 through 0xeb, inside the palette-fill loop.
 * Bytes from the pixel loop onward are identical, so the exact sample
 * registers and the red/green/blue accumulator schedules cannot change.
 *
 * Artifacts:
 *   build/doblur-exact-frame-inner-component-fill-order-axes.json
 *   build/doblur-exact-frame-inner-component-fill-order-manifest.json
 *   build/match-variants/doblur-exact-frame-inner-component-fill-order
 *
 * Disposition: conclusively pruned for B10 closure.  The six hashes represent
 * palette-loop spellings, not new pixel-loop structural islands.
 */
