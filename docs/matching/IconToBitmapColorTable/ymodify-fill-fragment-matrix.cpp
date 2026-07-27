/*
 * IconToBitmapColorTable, RVA 0xd32a0:
 * IconToBitmapYModify-derived do_fill structure and smaller macro boundaries.
 *
 * IconToBitmapYModify has a retained exact MAX and uses the same shared
 * do_fill join for solid and recolored dim runs.  This matrix transferred its
 * clipped-fill ownership without transferring its different whole-dispatcher
 * order: IconToBitmapColorTable retains its required no-clip arm, fill-before-
 * dim body order, do_fill/do_dim joins, count owner, and post-fill publication.
 *
 * Four exact-span source shapes were crossed independently with clean plus 50
 * declaration-forest states:
 *
 *   1. retained direct fill;
 *   2. YModify-adapted direct fill;
 *   3. a parameterized macro around the whole fill operation;
 *   4. a parameterized macro around only the clipped-fill body.
 *
 * The complete corrected matrix compiled 204/204 candidates in 143.43s and
 * was not truncated.  All four maxima preserve retail's 86-block topology:
 * 67 exact blocks, 19 size-only blocks, no target shifts, no flow-kind
 * mismatches, and no missing blocks.  All have 89/91 ordered relocations.
 *
 * Results:
 *
 *   retained direct:
 *     clean 82.295780%, best 83.866196%, size 1430
 *
 *   YModify-adapted direct:
 *     clean 83.455400%, best trial 38 at 83.995310%, size 1430
 *
 *   whole-fill macro:
 *     clean 82.295780%, best 83.737090%, size 1430
 *
 *   clipped-fill macro:
 *     clean 82.295780%, best 83.737090%, size 1430
 *
 * The two macro boundaries were byte/normalized-relocation identical in all
 * 51 paired states.  They matched the old direct form in only 30/51 states.
 * The adapted direct form matched the old direct form in only 23/51 states,
 * confirming that it exposes a genuinely different compiler-state orbit.
 *
 * The retained adapted body is represented below:
 */
#if 0
do_fill:
    if (clip == ICON_DRAW_NO_CLIP) {
        memset(row + X, gCTColor, count);
    } else if (clipY <= gCTY && gCTY <= gCTClipB
               && static_cast<i32>(X + count) > clipX && gCTClipR >= X) {
        i32 fillRight = X + count;
        if (clipX <= X) {
            if (gCTClipR >= fillRight)
                memset(row + X, gCTColor, count);
            else
                memset(row + X, gCTColor, (gCTClipR - X) + 1);
        } else {
            if (gCTClipR >= fillRight)
                memset(row + clipX, gCTColor, (count - clipX) + X);
            else
                memset(row + clipX, gCTColor, clipW);
        }
    }
    X = X + count;
    gCTRun = count;
    continue;
#endif

/*
 * The official trial-38 replay retained the paired object and raised the
 * generated hash-scoped MAX from 83.5024% to 83.9953% for
 * 34b660bf0557.0e13db56b47b.  The probe was removed before recording.
 *
 * Its complete relocation-owner audit is:
 *
 *   candidate/retail:
 *     gCTCnt 5/6, gCTSrc 14/15, gCTX0 2/3, gCTY 9/8;
 *     every other owner agrees, including gCTDst 6/6.
 *
 * Thus this family transfer repairs the former destination-publication
 * occurrence while leaving three missing occurrences and one excess Y
 * occurrence.  It is a real structural improvement, not exact closure.
 *
 * Artifacts:
 *   build/icon2bc-do-fill-boundaries-axes.json
 *   build/icon2bc-do-fill-boundaries-states-v2.json
 *   build/match-variants/icon2bc-do-fill-boundaries-v2/
 *   build/icon2bc-ymodify-fill-trial38-states-20260727.json
 *   build/tu-state-noise/
 *     icon2bc-ymodify-fill-trial38-record-max-20260727/
 *
 * Closed families were not replayed:
 *
 *   docs/matching-matrices/icon2bc-fill-arm-order-f45a3ba.tsv
 *   docs/matching-matrices/icon2bc-forward-sibling-99d936f.tsv
 *   docs/matching/IconToBitmapColorTable/row-visibility-inline.cpp
 *   docs/matching/IconToBitmapColorTable/literal-forward-exit-structures.cpp
 *   docs/matching/IconToBitmapColorTable/macro-era-reuse-boundaries.cpp
 *
 * Disposition: retain the YModify-adapted direct fill.  Do not retain either
 * macro; their complete state censuses are lower and do not establish a
 * reusable source boundary.
 */
