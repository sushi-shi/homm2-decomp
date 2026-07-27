/*
 * IconToBitmapColorTable, RVA 0xd32a0: inline sprite-right accessor.
 *
 * The same retail B2 ownership was routed through an Icon-family-style inline
 * boundary:
 *
 *   static inline i32 ColorTableSpriteRight(IconEntry* entry) {
 *       return entry->w + gCTX0;
 *   }
 *
 * A local spriteRight received the helper result before the clip predicate.
 * Complete clean + 50 forest-state sweep:
 *   build/icon2bc-inline-sprite-right-manifest.json
 *   build/match-variants/icon2bc-inline-sprite-right/
 *
 * Best: 82.185450%, size 1428, 89/91 relocations. The inline boundary changes
 * clean-state bytes but still CSEs the global access. It was removed.
 */

void IconToBitmapColorTable_inline_sprite_right_attempt() {
}
