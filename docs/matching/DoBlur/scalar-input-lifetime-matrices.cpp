/*
 * DoBlur reviewed scalar and input-lifetime descendants of the retained
 * red/blue/green component-table shape.
 *
 * Every structural version below was compiled against the clean state and a
 * complete 50-trial forest census. Generated declarations were not retained.
 *
 * 1. All six declaration orders for sample13, sample14, and sample15:
 *    build/match-variants/doblur-rbg-scalar-declaration-order-20260726
 *    The retained sample15/sample14/sample13 order remained best at
 *    95.729960%. sample14/sample15/sample13 reached 95.719406%.
 *
 * 2. Move only sample15 into the per-pixel loop:
 *    build/tu-state-noise/doblur-rbg-sample15-inner-20260726
 *    Best 94.801690%; rejected.
 *
 * 3. Declare all three scalar samples at their acquisition sites:
 *    build/tu-state-noise/doblur-rbg-all-scalars-inner-at-acquisition-20260726
 *    Best 95.373420% (trial 34); rejected. Register ownership changed to
 *    input=ECX, west4=EBP, north1=EDX, south4=ESI, away from retail.
 *
 * 4. All six declaration orders for input, remaining, and output:
 *    build/match-variants/doblur-rbg-pointer-order-20260726
 *    The first three orders were byte-identical to the retained maximum; the
 *    remaining three reached only 94.824900%.
 *
 * 5. Move input before versus after the scalar declarations:
 *    build/match-variants/doblur-rbg-input-relative-to-scalars-20260726
 *    Input-before trial 13 reproduced the retained 95.729960% object exactly:
 *    build/tu-state-noise/doblur-rbg-input-before-scalars-trial13-20260726
 *
 * 6. Split input declaration from assignment:
 *    build/tu-state-noise/doblur-rbg-input-split-declaration-20260726
 *    Byte/state-neutral; no improvement.
 *
 * 7. Introduce a natural destination pixel-base alias after scalar locals:
 *    build/tu-state-noise/doblur-rbg-input-base-alias-20260726
 *    No MAX improvement. Trial 20 is a structurally useful lower island:
 *    build/tu-state-noise/doblur-rbg-input-base-alias-trial20-20260726
 *    92.715190%, exact 1700-byte function length, 29 blocks with 28 exact and
 *    one size-only block, but frame 0xC60 rather than retail 0xC5C.
 *    Ownership was input=ESI, west4=EBP, north1=EDX, south4=ECX. It fixes the
 *    south4 register while swapping input/north1, so it remains clue evidence.
 *
 * 8. Cross the pixel-base alias with all six scalar declaration orders:
 *    build/match-variants/doblur-rbg-input-base-alias-scalar-orders-20260726
 *    No MAX improvement. Exact-length trial-20 descendants remained in the
 *    same wrong-frame orbit:
 *      build/tu-state-noise/doblur-rbg-input-base-alias-order-14-15-13-trial20-20260726
 *      build/tu-state-noise/doblur-rbg-input-base-alias-order-15-13-14-trial20-20260726
 *
 * 9. Move the pixel-base alias before scalar declarations:
 *    build/tu-state-noise/doblur-rbg-input-base-alias-before-scalars-20260726
 *    Trial 20 reached 94.430380% and exact length, but retained the 0xC60
 *    frame and restored the wrong input=ECX/south4=EDX ownership.
 *
 * 10. Combine the alias with acquisition-site scalar declarations:
 *     build/tu-state-noise/doblur-rbg-input-base-alias-all-scalars-inner-20260726
 *     Best 95.373420%; rejected.
 *
 * 11. Restore the retail-shaped 13+3 sample partition and retain the alias:
 *     build/tu-state-noise/doblur-old-partition-rbg-input-base-alias-20260726
 *     Best 94.497890%; rejected.
 *
 * 12. Restore that partition with blue/red/green tables and the alias:
 *     build/tu-state-noise/doblur-old-partition-brg-input-base-alias-20260726
 *     Best 94.601265%; it merely reproduced the earlier partition orbit.
 */

void DoBlur_scalar_input_lifetime_attempts() {
    /*
     * Representative alias version:
     *
     * u8* inputBase = destination->m_pixels;
     * u8* input = inputBase + rowOffset + BORDER_RADIUS;
     *
     * Representative acquisition-site version:
     *
     * i32 sample13 =
     *     static_cast<u32>(input[-BORDER_RADIUS]) << COMPONENT_INDEX_SHIFT;
     * i32 sample14 =
     *     static_cast<u32>(input[-SCREEN_WIDTH]) << COMPONENT_INDEX_SHIFT;
     * i32 sample15 =
     *     static_cast<u32>(input[SCREEN_WIDTH * BORDER_RADIUS])
     *     << COMPONENT_INDEX_SHIFT;
     */
}
