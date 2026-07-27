/*
 * FlipIconToBitmap initial-Y inline ownership matrix, 2026-07-27.
 *
 * Fresh relocation/disassembly review localized the two missing ordered
 * occurrences: retail reloads gFlipY inside the initial clipping decision and
 * again while publishing the first row.  The live object reuses the mutated
 * `y` formal at both sites.  Earlier setup and clip-bottom matrices changed
 * value ownership and CFG, but did not place these consumers behind sibling-
 * style inline boundaries.
 *
 * Three reviewed forms were tested:
 *
 *   - FlipNeedsClipping(...) reading gFlipY internally;
 *   - FlipPublishInitialRow(dest, pitch) using gFlipY;
 *   - both inline boundaries together.
 *
 * Each form was crossed with clean plus 50 forest states: 153/153 compiled,
 * source restored, and no wall-time truncation.
 *
 * Clean results:
 *
 *   clip helper only:      83.058360%, size 1262, 79/81 relocations;
 *   row helper only:       84.578250%, size 1264, 79/81 relocations;
 *   clip plus row helpers: 87.509285%, size 1251, 79/81 relocations.
 *
 * The strongest state was the row helper at forest trial 15:
 * 87.867370%, size 1251, 79/81 relocations.  The row-only clean object is a
 * credible one-byte-short structural island, while the combined clean object
 * is a strong near-MAX branch.  All retain the live 75-block graph versus
 * retail's 76 blocks, and neither helper forced the missing ordered reloads.
 *
 * Artifacts:
 *   build/iconf2b-initial-y-inline-axes-20260727.json
 *   build/iconf2b-initial-y-inline-manifest-20260727.json
 *   build/match-variants/iconf2b-initial-y-inline-20260727
 *
 * Disposition: preserve row-only and combined helpers as credible descendants
 * but retain no source, generated state, or MAX update.
 */
