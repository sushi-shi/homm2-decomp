/*
 * FlipIconToBitmapYModify, RVA 0xd9ce0: incremental clipped-literal
 * ownership, 2026-07-27.
 *
 * Three source structures were tested independently against clean plus 50
 * top-of-TU forest states.
 *
 * 1. Incremental right-clipped source adjustment:
 *
 *      gFYSrc = gFYSrc + gFYX;
 *      gFYSrc = gFYSrc - gFYClipR;
 *
 *    build/iconf2by-incremental-source-manifest.json
 *    build/match-variants/iconf2by-incremental-source/
 *
 *    Best: 92.426280%, size 1420, 144/144 relocations.
 *
 * 2. The source adjustment plus incremental both-sides-clipped skip:
 *
 *      pendingSkip = gFYRun;
 *      pendingSkip = pendingSkip - gFYX;
 *      pendingSkip = pendingSkip - clipWidth;
 *      pendingSkip = pendingSkip + gFYClipR;
 *
 *    build/iconf2by-incremental-tail-manifest.json
 *    build/match-variants/iconf2by-incremental-tail/
 *
 *    Best: 92.439680%, size 1420, 144/144 relocations.
 *
 * 3. Incremental skip alone:
 *
 *    build/iconf2by-incremental-skip-manifest.json
 *    build/match-variants/iconf2by-incremental-skip/
 *
 *    Best: 93.605896%, size 1420, 144/144 relocations, at trials 10,
 *    12, 19, 24, 31, and 47. This exceeds the previous 93.5925% MAX.
 *
 * The incremental skip matches FlipIconToBitmap's established ownership
 * pattern and is retained. The incremental source adjustment is a coherent
 * lower island, but is removed because it loses the stronger existing orbit.
 *
 * Official replay:
 *   build/tu-state-noise/
 *     iconf2by-incremental-skip-trial10-record-max-20260727/
 *
 * Trial 10 reproduced 93.605896%, size 1420, and 144/144 raw relocation
 * count. `tu_state_noise.py --record-max` restored the generated probe and
 * raised the generated baseline row to 93.6059% for source hash
 * 1df72f5245f5.aa0f82f3995a. The result is not exact because ordered
 * relocation identity and the remaining byte differences still fail.
 */

void FlipIconToBitmapYModify_incremental_literal_tail_attempts() {
}
