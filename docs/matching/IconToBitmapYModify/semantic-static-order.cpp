/*
 * IconToBitmapYModify, RVA 0xda270: semantic file-scope state order.
 *
 * The exact declarations were regrouped without changing names, types, DATA
 * addresses, or the function body:
 *
 *   core: s_entry, s_src, s_row, s_pitch, s_left, s_x, s_y, s_run, s_color
 *   clip: s_clipR, s_clipB
 *   dim:  s_dimPal, s_dimDst, s_spanCount, s_dimLen, s_loopIndex
 *
 * The old exact forest trial 39 moved to 99.225890%, size 1418. A complete
 * 200-trial top-of-TU forest census found 20 compiler states but no exact
 * closure. Every state retained the exact 64/64 CFG topology; the best state
 * remained trial 39.
 *
 * Evidence:
 *
 *   build/tu-state-noise/
 *       icon2by-semantic-static-order-200-20260727/
 *
 * This is a compiler-state result, not a semantic rejection. The previous
 * declaration order retains an audited 100% island. Do not adopt this order
 * unless a later search recovers exact closure.
 */

void IconToBitmapYModify_semantic_static_order_attempt() {
}
