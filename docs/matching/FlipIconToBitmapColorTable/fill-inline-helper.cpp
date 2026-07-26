// Experiment: remove the shared do_fill label by expressing the fill operation
// as a normal inline helper with two source-level call continuations.
// Artifact: build/match-variants/iconf2bc-no-do-fill-clean-20260726/
// Clean result: 62.231740%, size 1608 versus 1342, relocations 94/83.
// Disposition: rejected as a different and much worse structural orbit.

#if 0
static inline void FlipColorFillRun(
    i32 clip, i32 clipX, i32 clipY, i32 X, u32 count)
{
    if (clip == ICON_DRAW_NO_CLIP) {
        memset((gFCRow - count) + 1 + X, gFCColor, count);
    } else {
        i32 left;
        if (FlipColorRowVisible(clipY)
            && (left = (X - count) + 1, clipX <= left)
            && X <= gFCClipR) {
            if (clipX <= left)
                memset((gFCRow - count) + 1 + X, gFCColor, count);
            else
                memset(gFCRow + clipX, gFCColor, (X - clipX) + 1);
        }
    }
}

// Both the solid and recolor branches used this continuation.
FlipColorFillRun(clip, clipX, clipY, X, count);
X = X - count;
gFCRun = count;
continue;
#endif
