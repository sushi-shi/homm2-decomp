/*
 * IconToBitmapScale, retail RVA 0x000d2f90, size 0x179.
 * Narrow scale/geometry inline boundaries, 2026-07-27.
 *
 * Fresh comparison found exact 18/18 flow with 13 instruction-count-exact
 * blocks and five size-only blocks. The first byte divergence is entry
 * ownership: retail uses a 0x18-byte frame, retains scale in EBX and dest in
 * ESI, while clean source uses a 0x10-byte frame and reloads those values.
 * `od-frames` reports no /Od pair because this is an optimized function.
 *
 * Earlier work exhausted source/destination/scale aliases, whole geometry
 * aggregate initializers, and large native/slow-path helpers. This matrix
 * instead placed only the individual scale operations behind small inline
 * boundaries:
 *
 *     IconScaleIsNative(scale)
 *     IconScaleStep(scale)
 *     IconScaleSourceBase(scale, step)
 *     IconScaleSourceAdvance(step)
 *
 * Six reviewed arms comprised direct source, each of the four boundaries
 * alone, and all four boundaries together. Every arm was crossed with clean
 * plus all 50 top-of-TU forest states: 306/306 compilations completed in
 * 216.34 seconds and source was restored.
 *
 * Clean results:
 *
 *   direct and each single helper  75.695040%, 361 bytes, 4/4 relocations
 *   all four narrow helpers        80.248230%, 365 bytes, 4/4 relocations
 *
 * The combined helper declarations open target text SHA 99b16a4545dc4ffa,
 * a genuine cleaner 18-block island with the same 13 exact plus five
 * size-only census. Each single boundary is completely canonical at clean
 * state and produces direct hash 200c6b329efcae17.
 *
 * All six top-state censuses reach the durable 85.829790% ceiling at 365 or
 * 366 bytes with 4/4 ordered relocations. The helper arms converge on known
 * target text SHA 5919a0a10f23defd; the 366-byte direct arm reaches known
 * publication SHA e8f446cdf76e988c. No arm changes the 13/5 block census,
 * enters retail's frame/register ownership, reaches retail size 377, or
 * closes exact bytes.
 *
 * Artifacts:
 *
 *   build/icon2bs-scale-accessor-axes-20260727.json
 *   build/icon2bs-scale-accessor-top-states-20260727.json
 *   build/match-variants/icon2bs-scale-accessor-top-states-20260727/results.json
 *
 * Disposition: preserve the 99b16a4545dc4ffa clean object as a lower
 * structural island, but retain no helper or source change. The aggregate
 * declaration forest changes compiler state without recovering a surviving
 * retail inline boundary. MAX and generated configuration remain unchanged.
 */

void IconToBitmapScale_scale_geometry_inline_boundaries_attempt() {
}
