/*
 * IconToBitmapColorTable structural experiment: first differing block bodies
 * B2, B4, B7, and B9 after the 86/86 flow topology was established.
 *
 * Four reviewed axes covered the obvious source ownership choices:
 *
 *     clipW + clipX < entry->w + gCTX0
 *     entry->w + gCTX0 > clipW + clipX
 *
 *     gCTClipR = clipX + clipW - 1;
 *     gCTClipR = clipX + (clipW - 1);
 *
 *     i32 cmd;                 u32 cmd;
 *     i32 flags = 0;           i32 flags;
 *
 * The horizontal and vertical clip predicates were mirrored together, and the
 * right/bottom bound expressions were changed together.
 *
 * Complete matrix (2026-07-26):
 *   16 source shapes x (clean + 50 forest states)
 *   816/816 compiled in 643.39 seconds; source restored
 *   retail: size 1455, 91 relocations, 86 blocks
 *
 * All 16 source shapes compiled to the same text and relocation state for each
 * corresponding TU state.  The best score was therefore the known untouched
 * trial-11 island:
 *   82.776990%, size 1428, 89/91 relocations,
 *   69 exact blocks + 17 size-only, no flow mismatch.
 *
 * Lower-scoring structural islands were retained in the results rather than
 * pruned by score:
 *
 *   forest trial 2:
 *     81.061035%, size 1429, 90/91 relocations,
 *     70 exact blocks + 16 size-only, no flow mismatch.
 *
 *   forest trial 47:
 *     81.154930%, size 1436, 90/91 relocations,
 *     70 exact blocks + 16 size-only, no flow mismatch.
 *     This is the closest-size island in this matrix, 19 bytes short of retail.
 *
 * The result JSON retains each state's text bytes, normalized relocation
 * stream, and retail byte-difference census.  Generated declarations and
 * candidate source were not retained.
 *
 * Artifacts:
 *   build/match-variants/icon2bc-early-block-axes-20260726.json
 *   build/match-variants/icon2bc-early-block-matrix-20260726.json
 *   build/match-variants/icon2bc-early-block-islands-20260726/
 *
 * Disposition: these spellings are compiler-canonical and cannot explain the
 * first size divergences.  Do not land a source change.  The trial-2 and
 * trial-47 states remain lower-score structural clues; the historical 82.8122%
 * hash-scoped MAX is unchanged.
 */
