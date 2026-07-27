/*
 * DoBlur reused component-accumulator lifetime matrix, 2026-07-27.
 *
 * Prior staged-lookup experiments retained three distinct source accumulators,
 * and prior whole-channel objects wrapped each channel.  This experiment
 * instead modeled retail B10's register dataflow directly: one scalar is
 * reused for red, green, and blue, while a lookup index carries each completed
 * quantized channel forward.
 *
 * Representative pixel-scope form:
 *
 *     u32 componentSum = red term 0 + red term 1;
 *     // remaining red terms
 *     u32 lookupIndex =
 *         (componentSum & ~QUANTIZATION_MASK) << QUANTIZATION_SHIFT;
 *
 *     componentSum = green term 0 + green term 1;
 *     // remaining green terms
 *     lookupIndex += componentSum & ~QUANTIZATION_MASK;
 *
 *     componentSum = blue term 0 + blue term 1;
 *     // remaining blue terms
 *     lookupIndex += componentSum >> QUANTIZATION_SHIFT;
 *     *output++ = reinterpret_cast<u8*>(lookup)[lookupIndex];
 *
 * The matrix independently moved componentSum and lookupIndex from pixel
 * scope to row scope, producing four structural arms.  Every arm was crossed
 * with the clean state and 50 forest states: 204/204 compiled in 191.41
 * seconds, without truncation, and source was restored.
 *
 * All four arms were byte-identical within every state.  Clean remained
 * 92.854430%, size 1701, 43/43 relocations, and 29/29 blocks with B10 alone
 * size-only at 215 candidate instructions versus 216 retail.  Forest trials
 * 10 and 24 reproduced the known 95.729960%, size-1703 island in every arm.
 *
 * Artifacts:
 *   build/doblur-reused-accumulator-lifetime-axes-20260727.json
 *   build/doblur-reused-accumulator-lifetime-manifest-20260727.json
 *   build/match-variants/doblur-reused-accumulator-lifetime-20260727
 *
 * Disposition: this retail-looking source ownership is fully canonicalized by
 * MSVC 4.2 before B10 allocation.  Retain no source or MAX change.
 */

void DoBlur_reused_accumulator_lifetime_record() {
}
