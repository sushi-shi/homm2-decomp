/*
 * IconToBitmapScale, RVA 0xd2f90: copied-pixel inline boundary.
 *
 * Representative source:
 *
 *   static inline void CopyIconScalePixel(
 *       u8*& destination, u8*& source, i32 step
 *   ) {
 *       if (*source != 0)
 *           *destination = *source;
 *       destination++;
 *       source += step;
 *   }
 *
 * This was tested together with the clear-row helper:
 *   build/icon2bs-inline-clear-pixel-manifest.json
 *   build/match-variants/icon2bs-inline-clear-pixel/
 *
 * Complete clean + 50 sweep. Best: 79.191490%, size 386, 4/4
 * relocations. The helper materially expands the function and belongs to a
 * lower structural island. It was rejected and removed.
 */

void IconToBitmapScale_inline_copy_pixel_attempt() {
}
