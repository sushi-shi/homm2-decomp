/*
 * FlipIconToBitmap helper-boundary x setup x declaration-order x state cross,
 * 2026-07-27.
 *
 * Complete product:
 *   - historical or semantic function-static declaration order;
 *   - publication-interleaved or staged-field setup;
 *   - staged arithmetic, single expression, value helper, reference helper,
 *     pointer helper, or local pointer alias for pendingSkip;
 *   - direct/reversed predicate or two narrow inline row helpers;
 *   - clean plus five reproducible forest states, including the prior
 *     best-topology state.
 *
 * All 2 x 2 x 6 x 4 x 6 = 576 cells completed in 608.84 seconds. The source
 * was restored and the matrix was not truncated:
 *
 *   build/iconf2b-layered-helper-islands-axes-20260727.json
 *   build/iconf2b-layered-helper-islands-manifest-20260727.json
 *   build/source-variant-batch/iconf2b-layered-helper-islands-20260727/
 *
 * The highest-fuzzy result remained the previously known clean staged-setup
 * island: 89.310350%, size 1257, 82/81 relocations, 70 exact plus six
 * size-only blocks.
 *
 * The best topology was 86.347480%, size 1240, 81/81 relocations, exact
 * 76-block CFG, 71 exact plus five size-only blocks. It used the retained
 * publication-interleaved setup, staged pending arithmetic, the narrow row
 * helper, and forest trial 1. This reproduces but does not improve the prior
 * 71/5 structural orbit.
 *
 * Staged pending arithmetic and its single-expression form were byte-identical
 * in the highest-fuzzy clean setup. Value, reference, pointer, and local-pointer
 * ownership opened other islands but did not improve topology or relocations.
 * Direct and reversed row predicates were byte-identical. The inline row
 * boundary remains a credible mechanism but is not retained because its clean
 * source does not improve the current branch.
 */

#if 0
static inline u32 FlippedLiteralSkip(
    u32 command, i32 currentX, i32 clipWidth, i32 clipRight
) {
    u32 skip = command;
    skip -= currentX;
    skip -= clipWidth;
    skip += clipRight;
    return skip;
}

static inline void SetFlippedLiteralSkip(
    u32& skip, u32 command, i32 currentX, i32 clipWidth, i32 clipRight
) {
    skip = command;
    skip -= currentX;
    skip -= clipWidth;
    skip += clipRight;
}

static inline i32 FlipRowVisible(
    i32 clipTop, i32 currentY, i32 clipBottom
) {
    return clipTop <= currentY && currentY <= clipBottom;
}
#endif
