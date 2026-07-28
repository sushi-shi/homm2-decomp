/*
 * FlipIconToBitmapYModify, RVA 0xd9ce0: final direct-state/static-order attempt.
 *
 * Matrix:
 *   build/iconf2by-no-local-static-orders-trial60-20260727.json
 * Corrected results:
 *   build/source-variant-batch/
 *     iconf2by-no-local-static-orders-trial60-corrected-20260727/
 *
 * This attempt removed the incidental fillCount and pendingSkip locals.  The
 * dim destination was already published directly through s_dimDst.  The
 * right-clipped literal path used the compact expression found in the flipped
 * color-table sibling:
 *
 *   s_srcSkip = s_clipR + ((s_run - s_x) - clipW);
 *
 * One fixed function shape was compiled at the known trial-60 TU state against
 * 99 static declaration orders: the forward-relative order, the historical
 * flipped order, retail-address order, and every ordering/placement of the
 * three reverse-only state declarations among the common forward sequence.
 *
 * All declaration orders reached the same 93.243965% island: 1405 candidate
 * bytes, 146/144 relocations, and 58/59 blocks.  This is structurally and
 * numerically worse than the retained direct-s_dimDst island at 94.410190%
 * (1419 bytes, 144/144 relocations, 59/59 blocks).  Therefore the direct-local
 * source below was not retained.  The result is evidence that declaration
 * order does not rescue this particular combined source shape.
 */

void FlipIconToBitmapYModify_all_direct_state_attempt() {
    /*
     * Solid span:
     *
     * if (IconRowVisible(shear, clipY, s_y, s_clipB)) {
     *     if (clipX <= (s_x - s_run) + 1 && s_x <= s_clipR) {
     *         if (clipX <= (s_x - s_run) + 1) {
     *             memset((s_row - s_run) + 1 + s_x, s_color, s_run);
     *         } else {
     *             memset(s_row + clipX, s_color, (s_x - clipX) + 1);
     *         }
     *     }
     * }
     *
     * Literal join:
     *
     * do {
     *     if (s_x <= s_clipR) {
     *         s_dst = s_row + s_x;
     *         if (clipX <= left) {
     *             s_srcSkip = 0;
     *             s_spanCount = s_run;
     *             break;
     *         }
     *         s_spanCount = (s_x - clipX) + 1;
     *         s_srcSkip = s_run - s_spanCount;
     *     } else {
     *         s_src += s_x - s_clipR;
     *         s_dst = s_row + s_clipR;
     *         if (clipX <= (s_x - s_run)) {
     *             s_srcSkip = 0;
     *             s_spanCount = (s_run - s_x) + s_clipR;
     *             break;
     *         }
     *         s_srcSkip = s_clipR + ((s_run - s_x) - clipW);
     *         s_spanCount = clipW;
     *     }
     * } while (0);
     */
}
