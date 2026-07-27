/*
 * IconToBitmapColorTable, RVA 0xd32a0, retail size 1455:
 * literal-run clipping tail ownership.
 *
 * The retained single-iteration shared tail:
 *
 *   do {
 *       // reject paths assign cnt = 0 and break
 *   } while (0);
 *
 * was compared with a family-style nested conditional that initialized
 * cnt = 0 and entered the clipping body only when visible. Both received a
 * clean plus 50-state census.
 *
 * The retained form preserves exact 86-block flow and reaches 83.842720% in
 * this seed. The nested form tops out at 83.396710%, size 1418, 88/91
 * relocations, with 85 blocks, six flow-kind changes, and one missing block.
 *
 * Artifacts:
 *   build/icon2bc-literal-tail-manifest.json
 *   build/match-variants/icon2bc-literal-tail/
 *
 * Disposition: reject the nested form. The shared break tail is positive
 * structural evidence rather than incidental goto-like source.
 */
