/*
 * DoBlur structural experiment: revisit the exact-frame sample partition and
 * move its three scalar samples into the pixel loop.
 *
 * Attempted structure:
 *
 *     i32 samples[13]; // includes NORTH_FOUR_SAMPLE
 *     ...
 *     i32 sample13 = input[-BORDER_RADIUS] << 2; // west four
 *     i32 sample14 = input[-SCREEN_WIDTH] << 2;  // north one
 *     i32 sample15 = input[SCREEN_WIDTH * 4] << 2; // south four
 *
 * Row-scope plus all six inner declaration orders were each crossed with the
 * clean state and the same 50 target-adjacent TU-state probes.
 *
 * Result (2026-07-26):
 *   complete matrix: 7 source shapes x 51 states = 357/357
 *   inner 13/14/15, trial 40: 93.708860%, size 1705, relocs 43/43
 *   frame: candidate 0xc5c, retail 0xc5c
 *   CFG: 29/29; B9 and the complete sample-load prefix are exact
 *   registers: input=EDX, west4=EBP, north1=ESI, south4=ECX (all retail)
 *   B10: 218 instructions versus retail 216
 *
 * This is a valuable lower-scoring structural island: its first local B10
 * mismatch occurs only after all sample loads, in the final red scalar reads.
 * It was therefore used as the base for every exact-frame descendant matrix.
 *
 * Artifacts:
 *   build/doblur-exact-frame-inner-scalar-order-axes.json
 *   build/doblur-exact-frame-inner-scalar-order-manifest.json
 *   build/match-variants/doblur-exact-frame-inner-scalar-order
 *   build/tu-state-noise/doblur-exact-frame-inner-13-14-15-trial40
 *
 * Disposition: clue only.  Do not replace the retained alternate partition,
 * whose historical MAX is 95.552740%.
 */
