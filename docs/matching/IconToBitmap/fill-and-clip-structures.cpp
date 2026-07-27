/*
 * IconToBitmap solid-fill and clip-control structure census.
 *
 * Reviewed forms included a shared fill label, duplicated clipped/unclipped
 * calls, initialized versus publication-owned fill lengths, and all current
 * versus inverse relational spellings in the setup, solid, dim, and literal
 * clipping regions.  A representative shared call is:
 *
 *     if (clip == ICON_DRAW_NO_CLIP)
 *         fillLen = count;
 *     else
 *         fillLen = clippedLength;
 *     memset(fillDst, gIcColor, fillLen);
 *
 * Completed matrices (2026-07-26):
 *
 *   icon2b-fill-structure-clean-20260726
 *     3/3 source shapes; best 71.612640%, size 1253, 83/83 relocations.
 *
 *   icon2b-frame-fill-islands-20260726
 *     363/363 source/state variants; the uninitialized signed-length form
 *     reached 76.079670%, size 1252, 81/83 relocations.
 *
 *   icon2b-frame-elimination-clean{-v2}-20260726
 *     10 total reviewed clean variants; best 66.579670%, size 1304.
 *
 *   icon2b-relational-regions-clean-20260726
 *     16/16 predicate combinations; the current relations remained best at
 *     73.848900%, size 1249, 83/83 relocation count.
 *
 *   icon2b-relational-regions-islands40-20260726
 *     656/656 source/state variants.  The all-current predicate structure
 *     reached 78.414830%, size 1241, 83/83 relocation count.
 *
 * These axes do not explain the remaining mismatch.  Lower-scoring predicate
 * islands remain evidence but were not retained in reconstructed source.
 */

void IconToBitmap_fill_and_clip_attempts() {
}
