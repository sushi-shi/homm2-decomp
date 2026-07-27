/*
 * IconToBitmapScale, RVA 0xd2f90: clear-row inline boundary.
 *
 * Representative source:
 *
 *   static inline void ClearIconScaleRow(u8* pixels, i32 rowOffset) {
 *       memset(pixels + rowOffset, 0, SCALE_NATIVE_SIZE);
 *   }
 *
 * The clear loop called this helper. Complete clean + 50 sweep:
 *   build/icon2bs-inline-clear-row-manifest.json
 *   build/match-variants/icon2bs-inline-clear-row/
 *
 * Best: 85.829790%, size 365/366 depending on state, 4/4 relocations.
 * The inline boundary did not expose a new best orbit, so it was removed.
 */

void IconToBitmapScale_inline_clear_row_attempt() {
}
