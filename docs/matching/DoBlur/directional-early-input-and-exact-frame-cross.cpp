/*
 * DoBlur structural hybrids, 2026-07-27.
 *
 * The retained alternate partition and the directional flat sample layout
 * were crossed with both input lifetimes:
 *
 *     i32 samples[13];
 *     // either before or after the three sample scalars
 *     u8* input = destination->m_pixels + rowOffset + BORDER_RADIUS;
 *
 * The directional layout gives contiguous compass ownership:
 *
 *     west4..west1, east1..east4,
 *     north3, north2, south1, south2, south3
 *
 * Complete result: 2 * 2 * (clean + 50 forest states) = 204/204.
 * Both directional input lifetimes reach the same 95.738396% text, size
 * 1703, 43/43 relocations, and 29/29 CFG with 28 exact blocks plus B10
 * size-only.  The early-input hybrid canonicalizes to the same object at
 * trial 13 that the retained lifetime reaches at trial 10.  It therefore
 * does not repair the 0xC60 frame, input=ECX, south4=EDX ownership.
 *
 * The directional layout was next carried into the lower exact-frame parent:
 *
 *     i32 samples[13]; // contains north-four
 *     ...
 *     i32 sample13 = input[-4] << 2;       // west-four
 *     i32 sample14 = input[-640] << 2;     // north-one
 *     i32 sample15 = input[2560] << 2;     // south-four
 *
 * Its acquisition-order and directional slot layouts were first swept alone
 * (2 * 51 = 102/102), then crossed with the three table orders supported by
 * the prior register audit: red/blue/green, blue/red/green, green/red/blue.
 * The latter matrix completed 2 * 3 * 51 = 306/306 candidates.
 *
 * Best exact-frame result: 93.358650%, size 1706, 43/43 relocations,
 * acquisition-order slots plus red/blue/green tables at trial 34.  The best
 * directional arm was 93.270040%.  Both retain the retail 0xC5C frame and
 * exact 29-block topology, but neither fixes the accumulator schedule.
 *
 * Artifacts:
 *   build/doblur-directional-early-input-axes-20260727.json
 *   build/doblur-directional-early-input-manifest-20260727.json
 *   build/match-variants/doblur-directional-early-input-20260727/results.json
 *   build/doblur-exact-frame-directional-axes-20260727.json
 *   build/doblur-exact-frame-directional-manifest-20260727.json
 *   build/match-variants/doblur-exact-frame-directional-20260727/results.json
 *   build/doblur-exact-frame-directional-table-manifest-20260727.json
 *   build/match-variants/doblur-exact-frame-directional-table-20260727/results.json
 *
 * Disposition: preserve both hybrid families as structural evidence, but
 * retain neither in reconstructed source.  No audited exact closure appeared;
 * the durable live-source MAX remains 95.729960%.
 */

void DoBlur_directional_early_input_and_exact_frame_cross_record() {
}
