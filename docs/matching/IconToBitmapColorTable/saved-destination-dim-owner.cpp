/*
 * IconToBitmapColorTable, RVA 0xd32a0, retail size 1455:
 * saved-destination lifetime crossed with dim-pixel inline ownership.
 *
 * The direct loops and this family-derived boundary were each compiled in
 * clean state and 50 forest states:
 *
 *   static inline u8* ColorTableDimPixel(
 *       u8* dst, u8* palette, i32 dimGate
 *   ) {
 *       if (dimGate != 0)
 *           *dst = palette[*dst];
 *       return dst + 1;
 *   }
 *
 * Complete result: 102/102 compilations. The helper reaches 83.737090% in
 * clean state, whereas the direct source requires a TU-state island to reach
 * it. Both arms reach the same new best normalized object:
 * 83.866196%, size 1430, 89/91 relocations, exact 86-block flow.
 *
 * Artifacts:
 *   build/icon2bc-saved-dst-dim-helper-manifest.json
 *   build/match-variants/icon2bc-saved-dst-dim-helper/
 *
 * Disposition: retain the inline owner. It is a source-level structural
 * mechanism for the better allocation orbit and matches the inline-boundary
 * idiom used by the related icon blitters.
 */
