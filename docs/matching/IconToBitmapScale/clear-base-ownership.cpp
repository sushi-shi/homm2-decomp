/*
 * IconToBitmapScale, RVA 0xd2f90: clear-base lifetime islands.
 *
 * Retail spills tmp->m_pixels and reloads it for each 32-byte clear row while
 * retaining step in EBP.  The reconstructed source keeps a loop-external
 * tmpPixels snapshot live in a register.  Three credible ownership forms were
 * crossed with direct parameters versus the destination/scale input record:
 */

#if 0
// Existing external snapshot.
u8* tmpPixels = tmp->m_pixels;
do {
    memset(tmpPixels + rowOff, 0, SCALE_NATIVE_SIZE);
    rowOff += SCALE_NATIVE_SIZE;
} while (rowOff < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE);

// Direct member reload.
do {
    memset(tmp->m_pixels + rowOff, 0, SCALE_NATIVE_SIZE);
    rowOff += SCALE_NATIVE_SIZE;
} while (rowOff < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE);

// Per-iteration snapshot.
do {
    u8* tmpPixels = tmp->m_pixels;
    memset(tmpPixels + rowOff, 0, SCALE_NATIVE_SIZE);
    rowOff += SCALE_NATIVE_SIZE;
} while (rowOff < SCALE_NATIVE_SIZE * SCALE_WORK_BITMAP_SIZE);
#endif

/*
 * Clean screening:
 *
 *   build/icon2bs-input-clear-base-clean-axes-20260727.json
 *   build/match-variants/icon2bs-input-clear-base-clean-20260727/
 *
 * The input record plus direct reload opens a distinct 78.907800%/361-byte
 * island; its per-iteration snapshot reaches 79.695040%/360.  Both begin with
 * 12 exact plus six size-only blocks.  Without the input record the two forms
 * canonicalize to 75.531910%/354.
 *
 * Complete product: 2 input owners * 3 clear owners * (clean + 50 states)
 * = 306 compilations:
 *
 *   build/icon2bs-input-clear-base-states-manifest-20260727.json
 *   build/match-variants/icon2bs-input-clear-base-states-20260727/
 *
 * The direct-member and loop-internal families converge to a separate
 * 82.609930%/352-byte island with 13 exact plus five size-only blocks.  Only
 * the external snapshot reaches the existing 85.829790% ceiling.  Contrary
 * to the hypothesis, reloading the member shortens the function and does not
 * produce retail's 0x18 frame or scale/step register allocation.
 *
 * The lower islands remain recorded as evidence.  No source was retained and
 * MAX did not change.
 */

void IconToBitmapScale_clear_base_ownership_attempts() {
}
