/*
 * UNTRIED FlipIconToBitmap clipped-literal geometry ownership.
 *
 * These are planned descendants of the exact 76-block, 70/6 source and the
 * trial-5 71/5 compiler-state island.  The remaining literal size mismatches
 * are B57, B65, and B66.  Retail B66 visibly publishes one stack temporary
 * after each arithmetic step, but volatile produced the wrong 77-block graph
 * and is permanently rejected.
 *
 * Remaining legitimate structures:
 *   1. helper ownership of the whole right-clipped geometry tuple;
 *   2. out-reference helpers for literalCount, pendingSkip, source, and dst;
 *   3. a small result struct returned by value;
 *   4. do/while(0), one-label goto, and duplicated-success joins crossed with
 *      those ownership forms;
 *   5. pendingSkip hoisted beside literalCount versus scoped to the right
 *      clipping branch;
 *   6. signed/unsigned types crossed with the above, not tested alone;
 *   7. cached clipRight lifetime beginning before the overlap test versus
 *      inside the right-clipped branch.
 *
 * Value/reference/pointer helpers for pendingSkip alone, a local pointer alias,
 * and the row-visibility cross are complete and recorded in
 * layered-helper-state-cross.cpp. None improved the 71/5 topology island.
 */

#if 0
struct FlipLiteralGeometry {
    u32 copyCount;
    u32 sourceSkip;
};

static inline FlipLiteralGeometry RightClippedLiteralGeometry(
    u32 command, i32 currentX, i32 clipWidth, i32 clipRight
) {
    FlipLiteralGeometry result;
    result.sourceSkip = command;
    result.sourceSkip -= currentX;
    result.sourceSkip -= clipWidth;
    result.sourceSkip += clipRight;
    result.copyCount = clipWidth;
    return result;
}

static inline void PublishRightClippedLiteralGeometry(
    u32& copyCount,
    u32& sourceSkip,
    u32 command,
    i32 currentX,
    i32 clipWidth,
    i32 clipRight
) {
    sourceSkip = command;
    sourceSkip -= currentX;
    sourceSkip -= clipWidth;
    sourceSkip += clipRight;
    copyCount = clipWidth;
}
#endif
