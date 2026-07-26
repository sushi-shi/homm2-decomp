/*
 * DoBlur structural experiment: change which value occupies the second
 * spilled-array slot while preserving the 13-array-plus-3-scalar frame.
 *
 * Retained partition:
 *
 *     i32 samples[SPILLED_ARRAY_SAMPLE_COUNT];
 *     i32 sample15; // south four rows
 *     i32 sample14; // north one row
 *     i32 sample13; // north four rows
 *
 *     sample13 = input[-SCREEN_WIDTH * BORDER_RADIUS] << 2;
 *     samples[IDX(WEST_FOUR_SAMPLE)] = input[-BORDER_RADIUS] << 2;
 *
 * The former structure spilled north-four and kept west-four scalar.  The new
 * structure does the reverse.  Names were corrected before the final census,
 * and all uses preserve the same blur semantics.
 *
 * Result (2026-07-26):
 *   clean fuzzy: 92.854430%, size 1701
 *   best of 50 target-adjacent states: 95.552740%, trials 10 and 24, size 1703
 *   CFG: same 29-block family; B10 is 217 versus retail 216 instructions
 *   relocation targets: 43/43
 *
 * Artifacts:
 *   build/doblur-radius4-scalar-partition-target-50.json
 *   build/tu-state-noise/doblur-radius4-scalar-partition-target-50
 *   build/doblur-named-alt-partition-target-50.json
 *   build/tu-state-noise/doblur-named-alt-partition-target-50
 *   build/tu-state-noise/doblur-named-alt-partition-trial10-recorded
 *
 * Disposition: retained.  The correctly named source reproduces the stronger
 * orbit, emits a near-retail clean size, reduces the sole non-exact block to
 * one excess instruction, and raises historical MAX to 95.5527%.
 */
