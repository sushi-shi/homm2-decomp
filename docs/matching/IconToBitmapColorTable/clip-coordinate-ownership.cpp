/*
 * IconToBitmapColorTable structural experiment: clipping-coordinate lifetime
 * and global publication ownership.
 *
 * Retail/candidate relocation census before the experiment:
 *                         retail  retained source
 *   gCTX0                    3          2
 *   gCTY                     8          9
 *   gCTCnt                   6          5
 *   gCTSrc                  15         14
 *
 * Six initial structural arms crossed local X versus published gCTX0 at the
 * first clipping comparison with these Y lifetimes:
 *
 *     // Existing source
 *     gCTY = entry->y + y;
 *     if (clip != ICON_DRAW_NO_CLIP) {
 *         i32 currentY = gCTY;
 *
 *     // New higher-scoring island
 *     i32 currentY = entry->y + y;
 *     gCTY = currentY;
 *     if (clip != ICON_DRAW_NO_CLIP) {
 *
 *     // Direct global reads
 *     gCTY = entry->y + y;
 *     if (clip != ICON_DRAW_NO_CLIP) {
 *         // condition reads gCTY
 *
 * Complete initial census (2026-07-26):
 *   six source arms x (clean + 50 top-of-TU forest states)
 *   306/306 compiled; source restored
 *   snapshot-before-publication clean: 83.328636%, size 1410, 86/91 relocs
 *   best snapshot island: 83.645540%, size 1410 or 1411, 86/91 relocs
 *   best states: trials 2, 3, 13, and 50
 *   all arms retained the exact 86-block flow family
 *
 * The raw trial-13 object shows exactly five missing global accesses: one each
 * for gCTX0, gCTCnt, and gCTSrc, plus two for gCTY.  Replacing the condition's
 * local Y reads with gCTY restores the old 89-relocation topology but also
 * returns to the old byte orbit.  A complete four-arm descendant census
 * (204/204 variants) did not combine the 83.645540% byte island with that
 * topology.
 *
 * Artifacts:
 *   build/match-variants/icon2bc-clip-ownership-axes-20260726.json
 *   build/match-variants/icon2bc-clip-ownership-islands-20260726/
 *   build/match-variants/icon2bc-clip-global-descendants-axes-20260726.json
 *   build/match-variants/icon2bc-clip-global-descendants-islands-20260726/
 *   build/tu-state-noise/icon2bc-clip-snapshot-trial13-20260726/
 *
 * Disposition: retain snapshot-before-publication/state-13 as an active clue
 * branch.  It breaks the previous score ceiling but is not landed because its
 * wider local lifetime contradicts retail's later global reloads.  Continue
 * by layering the proven inline byte-reader boundary and by restoring the
 * missing gCTCnt publication read without discarding this island for score.
 */
