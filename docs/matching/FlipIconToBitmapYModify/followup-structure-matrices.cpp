/*
 * FlipIconToBitmapYModify, RVA 0xd9ce0: follow-up structural matrices,
 * 2026-07-27.
 *
 * Each source structure below was restored after its complete clean + 50
 * top-of-TU forest-state sweep. None exceeded the retained incremental-skip
 * island at 93.605896%.
 *
 * 1. Sibling-style local literal-count owner
 *
 *      u32 literalCount;
 *      // Each clipping arm assigns literalCount.
 *    copy_literal:
 *      gFYDimLen = literalCount;
 *
 *    build/tu-state-noise/iconf2by-local-literal-count-20260727/
 *    Best: 92.225204%, size 1399. This is a distinct lower orbit.
 *
 * 2. Sibling-style local right-edge owner
 *
 *      i32 right = gFYClipR;
 *      gFYSrc = gFYSrc + (gFYX - right);
 *      u8* rightDst = gFYRow + right;
 *
 *    build/tu-state-noise/iconf2by-local-right-owner-20260727/
 *    Best: 93.605896%. The useful states are byte-identical to the retained
 *    global-right-edge spelling, so the extra local was removed.
 *
 * 3. Both loop relational orders, generated as a reviewed permuter product
 *
 *      gFYDimIdx < gFYDimLen  ->  gFYDimLen > gFYDimIdx
 *
 *    Applied to both the dim-palette and literal-copy loops:
 *    build/iconf2by-loop-relational-orders-manifest.json
 *    build/match-variants/iconf2by-loop-relational-orders/
 *
 *    Best: 93.605896%. All corresponding state IDs are unchanged. The
 *    apparent `jl` versus `jg` block-listing difference is MSVC operand
 *    canonicalization, not evidence for a countdown loop.
 *
 * 4. Direct clipW parameter ownership
 *
 *    Removed the clipWidth alias and used clipW in setup and the clipped
 *    literal tail:
 *    build/tu-state-noise/iconf2by-direct-clipw-owner-20260727/
 *
 *    Best: 93.605896%. The strongest state is unchanged; this does not alter
 *    the setup register lifetime that differs from retail.
 *
 * 5. Nested shared inline shear accessor
 *
 *      static inline i8 IconShearAt(i8* shear) {
 *          return shear[gFYY];
 *      }
 *
 *    Used from setup, IconRowVisible, and row advance:
 *    build/tu-state-noise/iconf2by-shared-shear-inline-20260727/
 *
 *    Best: 88.257370%, size 1416. This creates a substantially lower orbit,
 *    so the helper was removed.
 */

void FlipIconToBitmapYModify_followup_structure_attempts() {
}
