/*
 * IconToBitmapColorTable, RVA 0xd32a0, retail size 1455:
 * run-position inline ownership.
 *
 * A reviewed helper was applied independently to the negative skip run, the
 * fill-run tail, and every run tail:
 *
 *   static inline void AdvanceColorTableRun(i32& position, u32 count) {
 *       position = position + count;
 *   }
 *
 * Complete matrix: four source arms times clean plus 50 forest states,
 * 204/204 compilations.
 *
 * Artifact:
 *   build/match-variants/icon2bc-run-advance-inline/
 *
 * Every helper subset entered the same 67.565730%, size-1491 orbit. The
 * direct source reached 82.776990%, size 1428, with 89/91 relocations and
 * exact 86-block flow. The helper versions were rejected.
 */
