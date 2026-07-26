/*
 * DoBlur structural experiment: fold the quantized channel sums into one
 * lookup index as each channel finishes.
 *
 * Attempted body:
 *
 *     // after the red accumulation
 *     u32 lookupIndex =
 *         (redSum & ~QUANTIZATION_MASK) << QUANTIZATION_SHIFT;
 *
 *     // after the green accumulation
 *     lookupIndex += greenSum & ~QUANTIZATION_MASK;
 *
 *     // after the blue accumulation
 *     lookupIndex += blueSum >> QUANTIZATION_SHIFT;
 *     *output++ = reinterpret_cast<u8*>(lookup)[lookupIndex];
 *
 * This is supported by retail's dataflow: it masks and shifts the completed
 * red accumulator before building green, and masks/adds green before building
 * blue.  The arm retained the exact 29-block CFG and 43/43 relocations.
 *
 * Result (2026-07-26):
 *   clean fuzzy: 91.265820%, size 1708
 *   best of 50 target-adjacent states: 93.040085%, size 1707
 *   best trials: 18 and 19
 *   palette declaration/update descendants: four complete 51-state arms
 *
 * Artifacts:
 *   build/doblur-staged-descendants-axes.json
 *   build/doblur-staged-descendants-manifest.json
 *   build/match-variants/doblur-staged-descendants
 *   build/doblur-staged-index-trial1.json
 *
 * Disposition: rejected from the live source.  This is a credible alternate
 * structural orbit, so its lower-scoring descendants and paired artifacts are
 * preserved, but none exceeds the retained 93.411390% MAX.
 */
