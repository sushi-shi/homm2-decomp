/*
 * DoBlur missing structural cross, 2026-07-27.
 *
 * This matrix combines four pieces that had previously been tested only in
 * separate parents:
 *
 *   - the retail-size 0xC5C frame's 13+3 sample partition, where north-four
 *     belongs to samples[] and west-four, north-one, and south-four are the
 *     three scalar samples;
 *   - the early input lifetime (input is declared after samples[] and before
 *     the three sample scalars);
 *   - blue/red/green component-table declaration order;
 *   - row-scope scalar samples in all six declaration orders.
 *
 * Representative attempted source:
 *
 *     BlurComponentTable blueComponents;
 *     BlurComponentTable redComponents;
 *     BlurComponentTable greenComponents;
 *     ...
 *     i32 samples[SPILLED_ARRAY_SAMPLE_COUNT];
 *     u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
 *     i32 sample14; // north one row
 *     i32 sample13; // west four columns
 *     i32 sample15; // south four rows
 *
 *     samples[IDX(NORTH_FOUR_SAMPLE)] =
 *         static_cast<u32>(input[-SCREEN_WIDTH * BORDER_RADIUS])
 *         << COMPONENT_INDEX_SHIFT;
 *     ...
 *     sample13 =
 *         static_cast<u32>(input[-BORDER_RADIUS]) << COMPONENT_INDEX_SHIFT;
 *
 * Complete matrix:
 *
 *   6 scalar orders * (clean + 50 forest states) = 306/306 candidates
 *   elapsed 313.66 seconds; no wall-time truncation
 *
 * Best state per scalar order:
 *
 *   sample14/sample13/sample15  94.080170%, size 1707, trial 38
 *   sample15/sample13/sample14  94.059074%, size 1707, trial 38
 *   sample15/sample14/sample13  92.909290%, size 1701, trial 50
 *   sample14/sample15/sample13  92.877640%, size 1707, trial 46
 *   sample13/sample15/sample14  92.700424%, size 1707, trial 9
 *   sample13/sample14/sample15  92.611820%, size 1702, trial 50
 *
 * The best retained island keeps the retail 0xC5C frame, the 29-block CFG,
 * and 28 exact-size blocks plus B10 size-only.  Its complete sample-load
 * prefix has the intended ownership: input=EDX, west4=EBP, north1=ESI, and
 * south4=ECX.  All 43 relocation identities/addends are present in order, but
 * source offsets diverge first at gpBufferPalette (candidate +0xB2, retail
 * +0xB5), and the function remains seven bytes too long.  It is therefore a
 * genuine lower structural island, not exact closure.
 *
 * Artifacts:
 *   build/doblur-exact-frame-early-brg-row-scalar-order-axes-20260727.json
 *   build/doblur-exact-frame-early-brg-row-scalar-order-manifest-20260727.json
 *   build/match-variants/doblur-exact-frame-early-brg-row-scalar-order-20260727
 *   build/tu-state-noise/
 *     doblur-exact-frame-early-brg-row-order-14-13-15-trial38-20260727
 *   build/doblur-exact-frame-early-brg-row-order-14-13-15-trial38-state.json
 *
 * Disposition: preserve the best paired object/diff and this source-shape
 * ledger as clue evidence.  Do not replace the stronger live-source branch
 * and do not advance its durable MAX.
 */

void DoBlur_exact_frame_early_brg_row_scalar_order_cross_record() {
}
