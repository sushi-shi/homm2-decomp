/*
 * DoBlur inline-boundary experiment: extract the final quantized lookup.
 *
 * Reviewed helper and call:
 *
 *     static inline u8 BlurLookupColor(
 *         BlurLookupRow* lookup, u32 red, u32 green, u32 blue
 *     ) {
 *         return lookup[red & ~QUANTIZATION_MASK]
 *                      [(green & ~QUANTIZATION_MASK)
 *                       + (blue >> QUANTIZATION_SHIFT)];
 *     }
 *
 *     *output++ = BlurLookupColor(lookup, redSum, greenSum, blueSum);
 *
 * Result (2026-07-26):
 *   clean fuzzy: 90.375530%, size 1706
 *   first forest state: 88.609700%, size 1708
 *   ordered relocations: 43/43
 *
 * Artifact:
 *   build/tu-state-noise/doblur-inline-lookup-clean
 *
 * Disposition: rejected.  The helper inlines completely, but its clean bytes
 * are exactly a state already reached by the unchanged source's island census
 * (state 96c5523c860af979).  It supplies no new target orbit.
 */
