/*
 * IconToBitmapColorTable structural experiment: recover the row-visibility
 * inline boundary used by the related icon blitters.
 *
 * The retained source spells the three sites directly:
 *
 *     clipY <= gCTY && gCTClipB >= gCTY
 *     gCTY < clipY || gCTClipB < gCTY
 *
 * Four helper arms tested both comparison orders and either the two positive
 * sites or all three sites:
 *
 *     static inline i32 ColorTableRowVisible(i32 clipTop) {
 *         return clipTop <= gCTY && gCTClipB >= gCTY;
 *     }
 *
 *     static inline i32 ColorTableRowVisible(i32 clipTop) {
 *         return clipTop <= gCTY && gCTY <= gCTClipB;
 *     }
 *
 * The negative site used:
 *
 *     if (!ColorTableRowVisible(clipY)) {
 *
 * Clean results (2026-07-26):
 *   manual conditions: 82.154930%, size 1428
 *   each helper arm:   82.389670%, size 1428
 *
 * Complete Cartesian census:
 *   five source arms x (clean + 50 top-of-TU forest states)
 *   255/255 compiled; the source was restored
 *   best manual arm: 82.776990%, size 1428, state trial 50
 *   best helper arm: 82.654930%, size 1435, state trials 1, 7, and 30
 *   retained CFG family: 86/86 blocks, 69 exact and 17 size-only
 *
 * Artifacts:
 *   build/match-variants/icon2bc-row-visible-axes-20260726.json
 *   build/match-variants/icon2bc-row-visible-clean-20260726/
 *   build/match-variants/icon2bc-row-visible-islands-root-20260726.json
 *   build/match-variants/icon2bc-row-visible-islands-root-20260726/
 *
 * Disposition: preserve this as a credible family-derived structural orbit,
 * but do not land it in reconstructed source.  Neither the clean helper nor
 * any of its 50 states exceeded the current hash-scoped 82.8122% MAX.
 */
