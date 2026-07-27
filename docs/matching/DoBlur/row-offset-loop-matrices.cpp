/*
 * DoBlur reviewed row-offset and row-loop descendants of the retained
 * red/blue/green component-table shape.
 *
 * Each structural version was compiled against the clean state and a complete
 * 50-trial forest census. None improved the retained 95.729960% maximum.
 *
 * 1. Split rowOffset declaration from assignment:
 *    build/tu-state-noise/doblur-rbg-row-offset-split-20260726
 *    Byte/state-neutral.
 *
 * 2. Widen rowOffset's lifetime by declaring it before the guarded loop:
 *    build/tu-state-noise/doblur-rbg-row-offset-outer-lifetime-20260726
 *    Best 95.194090%; rejected.
 *
 * 3. Recompute rowOffset from y inside the guarded loop:
 *    build/tu-state-noise/doblur-rbg-row-offset-from-y-20260726
 *    Byte/state-neutral.
 *
 * 4. Cross both row-tail statement orders with clean plus 50 states:
 *    build/match-variants/doblur-rbg-row-tail-order-20260726
 *    The y-first tail reached 95.289030%, size 1704:
 *    build/tu-state-noise/doblur-rbg-row-tail-y-first-trial10-20260726
 *    It retained input=ECX, north1=ESI, south4=EDX and did not repair the
 *    decisive ECX/EDX ownership swap. The committed rowOffset-first tail was
 *    restored after inspection.
 *
 * At the retained maximum, candidate setup loads rowOffset in EDX and forms
 * input in ECX; retail loads rowOffset in ECX and forms input in EDX. That
 * rowOffset register is later reused for south4, explaining the same ECX/EDX
 * swap at the first important body divergence. The tested lifetime and tail
 * spellings do not enter the retail register orbit.
 */

void DoBlur_row_offset_loop_attempts() {
    /*
     * Split declaration:
     *   i32 rowOffset;
     *   rowOffset = BORDER_RADIUS * SCREEN_WIDTH;
     *
     * Recomputed form:
     *   i32 rowOffset = y * SCREEN_WIDTH;
     *
     * Alternate tail:
     *   y++;
     *   rowOffset += SCREEN_WIDTH;
     */
}
