/*
 * IconToBitmapColorTable, RVA 0xd32a0: clipped dim-count ownership.
 *
 * Retail publishes the pre-clipped count to gCTCnt in the X < clipX arm,
 * computes the clipped value, then publishes that final value at the join.
 * The retained source contains both publications, but MSVC deletes the first.
 *
 * Two developer-plausible dataflow versions were tested:
 *
 *   u32 clippedCount;
 *   clippedCount = right <= clipRight
 *       ? (count - clipX) + X : clipW;
 *   count = clippedCount;
 *   cn = count;
 *
 * and the closer retail-register form:
 *
 *   if (right <= clipRight)
 *       cn = (count - clipX) + X;
 *   else
 *       cn = clipW;
 *
 * Each shape received a complete clean + 50 forest-state sweep:
 *   build/icon2bc-clipped-count-owner-manifest.json
 *   build/match-variants/icon2bc-clipped-count-owner/
 *   build/icon2bc-direct-cn-owner-manifest.json
 *   build/match-variants/icon2bc-direct-cn-owner/
 *
 * Both shapes are byte-identical to the retained source within corresponding
 * states. Best: 82.776990%, size 1428, 89/91 relocations. Neither preserves
 * the missing gCTCnt publication, so both were removed and MAX is unchanged.
 */

void IconToBitmapColorTable_clipped_count_owner_attempts() {
}
