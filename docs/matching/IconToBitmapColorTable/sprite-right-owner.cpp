/*
 * IconToBitmapColorTable, RVA 0xd32a0: explicit sprite-right ownership.
 *
 * Retail B2 reloads gCTX0 when forming the sprite's right edge. The retained
 * expression names gCTX0, but MSVC CSEs it back to local X and therefore emits
 * no relocation. This attempt materialized the expression:
 *
 *   i32 spriteRight = entry->w + gCTX0;
 *   if (X < clipX || clipW + clipX < spriteRight || ...)
 *
 * Complete clean + 50 forest-state sweep:
 *   build/icon2bc-sprite-right-manifest.json
 *   build/match-variants/icon2bc-sprite-right/
 *
 * Best: 82.185450%, size 1428, 89/91 relocations. The source opens a
 * distinct byte island, but the compiler still folds away the gCTX0 reload.
 * It was removed; the 82.8122% hash-scoped MAX is unchanged.
 */

void IconToBitmapColorTable_sprite_right_owner_attempt() {
}
