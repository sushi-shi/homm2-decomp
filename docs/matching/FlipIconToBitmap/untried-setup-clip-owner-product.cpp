/*
 * UNTRIED FlipIconToBitmap setup/clip ownership products.
 *
 * These remain hypotheses, not evidence.  Do not cite them as rejected until
 * each legitimate source shape has received a clean compile and the same
 * requested TU-state census.
 *
 * Current target:
 *   B2 is one instruction larger in retail than the best trial-5 topology
 *   island; B7 is two instructions smaller.  The setup must be judged as one
 *   product because distant dim/literal changes have already changed B0-B7.
 *
 * Remaining axes:
 *   1. clipWidth and clipBottom lifetimes, independently scoped;
 *   2. snapshot versus direct reads of entry->h and entry->y;
 *   3. positive containment versus negative outside predicate, preserving the
 *      same 76-block CFG;
 *   4. a narrow inline helper owning only the four-value containment test;
 *   5. clip-bound publication helper returning no values versus out-references;
 *   6. exhaustive declaration orders on the current 70/6 source, not an older
 *      function hash;
 *   7. clean and selected state-5/state-7 layering for every surviving shape.
 */

#if 0
// Local bound lifetimes.
i32 clipRight = clipX + clipW - 1;
i32 clipBottom = clipY + clipH - 1;
if (s_left < clipX || clipRight < s_left + width || s_y < clipY
    || clipBottom < entry->h + s_y) {
    clip = ICON_DRAW_CLIP;
    s_clipR = clipRight;
    s_clipB = clipBottom;
}

// Narrow predicate boundary.
static inline i32 FlipNeedsClip(
    i32 left, i32 width, i32 currentY, i32 height,
    i32 clipX, i32 clipY, i32 clipWidth, i32 clipHeight
) {
    return left < clipX || clipX + clipWidth < left + width
        || currentY < clipY || clipY + clipHeight < currentY + height;
}
#endif
