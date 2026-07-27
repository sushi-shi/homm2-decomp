/*
 * IconToBitmap dim-run ownership, count, and palette census.
 *
 * Reviewed source structures included:
 *
 *     u8* palette = reinterpret_cast<u8*>(uDimPal)
 *         + (flags & ICON_RLE_DIM_LEVEL_MASK)
 *             * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
 *
 *     u32 paletteLevel = (flags & ICON_RLE_DIM_LEVEL_MASK) >> 2;
 *     u8* palette = uDimPal[0][paletteLevel];
 *
 * Count publication was independently varied for clipped and unclipped loops:
 * direct loop counters, late aliases, and publication before/after right-bound
 * reloads.  Palette declaration/assignment and direct lookup ownership were
 * also crossed with the entry and Y-lifetime families.
 *
 * Completed matrices (2026-07-26):
 *
 *   icon2b-dim-publication-cartesian-20260726
 *     164/164 variants; single left-clipped publication plus a late count
 *     alias reached 78.414830%, size 1241, 83/83 relocation count.
 *
 *   icon2b-independent-dim-count-{clean,islands}-20260726
 *     4 clean and 484 state variants; best 76.008240%, size 1225, 79/83.
 *
 *   icon2b-direct-palette-islands-20260726
 *     726/726 variants; best 75.980770%, size 1225, 79/83.
 *
 *   icon2b-palette-lifetime-clean-20260726
 *     24/24 clean shapes; best 75.200550%, size 1228, 80/83.
 *
 *   icon2b-dim-left-publication-{clean,islands}-20260726
 *     122 total variants; best 74.840660%, size 1252, 81/83.
 *
 * No exact island appeared.  In particular, exact or near-exact size alone did
 * not repair ordered relocation ownership.
 */

void IconToBitmap_dim_and_palette_attempts() {
}
