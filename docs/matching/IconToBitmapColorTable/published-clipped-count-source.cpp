/*
 * IconToBitmapColorTable, RVA 0xd32a0:
 * use the published dim count in the left-clipped count calculation.
 *
 * The retail clipped-dim arm contains two gCTCnt relocations: it publishes
 * the original count before comparing against the right clip boundary, then
 * publishes the selected clipped count at the join.  The retained source had
 * both assignments, but MSVC removed the first because the calculation read
 * the local count:
 *
 *     gCTCnt = count;
 *     if (right <= clipRight)
 *         count = (count - clipX) + X;
 *
 * A developer-plausible owner-preserving form instead reads the published
 * value:
 *
 *     gCTCnt = count;
 *     if (right <= clipRight)
 *         count = (gCTCnt - clipX) + X;
 *
 * The retained local form and the published-owner form were crossed with the
 * clean state plus 50 top-of-TU declaration-forest states.  All 102 cells
 * compiled in 82.43 seconds; source was restored.
 *
 * Results:
 *
 *   local-count best:      83.983570%, size 1430, 89/91 relocations
 *   published-count clean: 83.877940%, size 1456, 91/91 relocations
 *   published-count best:  83.936620%, size 1449, 91/91 relocations
 *
 * Every best island preserves the exact 86-block CFG with 67 exact blocks
 * and 19 size-only blocks.  The published owner is a valuable lower
 * structural island: it restores the complete relocation count and is one
 * byte from retail size in the clean state.  It remains unproven because the
 * ordered relocation stream and bytes are not exact, so this single change
 * is not retained by itself.  It should be crossed with the coordinated
 * shared fill/dim macro experiment rather than discarded on fuzzy score.
 *
 * Artifacts:
 *   build/icon2bc-published-clipped-count-axes-20260727.json
 *   build/icon2bc-published-clipped-count-manifest-20260727.json
 *   build/match-variants/icon2bc-published-clipped-count-20260727/
 */

void IconToBitmapColorTable_published_clipped_count_source_record() {
}
