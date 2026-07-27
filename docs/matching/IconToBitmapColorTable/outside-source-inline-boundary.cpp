/*
 * IconToBitmapColorTable, RVA 0xd32a0:
 * clipped literal-source inline boundary.
 *
 * IconToBitmapYModify closes exactly with a narrow source-address helper in
 * its right-outside copy arm.  The color-table sibling has the corresponding
 * clipped source adjustment:
 *
 *   static inline u8* ColorTableOutsideSource(
 *       u8* src, i32 currentX, i32 clipX
 *   ) {
 *       return src + (clipX - currentX);
 *   }
 *
 *   gCTSrcCopy = ColorTableOutsideSource(gCTSrc, X, clipX);
 *
 * The helper was tested in clean state plus all 50 top-of-TU declaration-
 * forest states.  The complete census produced 13 normalized states.  Trial
 * 8, repeated by nine other trials, raised the hash-scoped maximum from
 * 83.8662% to 83.983570%.  The retained best object is size 1430 with 89/91
 * relocations and exact 86-block flow (67 exact-size, 19 size-only blocks).
 * This is a stronger compiler-state island, not exact closure.
 *
 * Artifacts:
 *   build/tu-state-noise/icon2bc-outside-source-inline/
 *   build/icon2bc-outside-source-inline-states.json
 *   build/tu-state-noise/icon2bc-outside-source-inline-trial8-record/
 *
 * The trial-8 replay recorded MAX for effective-source hash
 * ada9fa938c59.0e13db56b47b.  The generated declaration forest was removed;
 * only the family-derived inline boundary is retained.
 */

void IconToBitmapColorTable_outside_source_inline_boundary_attempt() {
}
