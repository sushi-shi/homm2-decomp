/*
 * IconToBitmap, RVA 0xd0570, and IconToBitmapColorTable, RVA 0xd32a0:
 * shared forward-RLE clipped-fill fragment, 2026-07-27.
 *
 * The two forward renderers have the same clipped solid-run operation after
 * substituting their per-TU row visibility, destination, X, color, count, and
 * clip owners.  This experiment represents that common source-era boundary
 * once in BASE/IconRleFill.h:
 */
#if 0
#define H2_ICON_RLE_CLIPPED_FILL(                                                               \
    rowVisible, row, currentX, color, runLength, clipX, clipW, clipRight)                        \
    if (rowVisible && static_cast<i32>(currentX + runLength) > clipX && clipRight >= currentX) { \
        i32 fillRight = currentX + runLength;                                                     \
        if (clipX <= currentX) {                                                                  \
            if (clipRight >= fillRight)                                                           \
                memset(row + currentX, color, runLength);                                         \
            else                                                                                  \
                memset(row + currentX, color, (clipRight - currentX) + 1);                        \
        } else {                                                                                  \
            if (clipRight >= fillRight)                                                           \
                memset(row + clipX, color, (runLength - clipX) + currentX);                       \
            else                                                                                  \
                memset(row + clipX, color, clipW);                                                 \
        }                                                                                         \
    }
#endif

/*
 * The macro has no do/while wrapper because each caller deliberately uses it
 * as the body of an `else`, producing the same `else if` CFG as the expanded
 * source.  The dedicated header is included only by Icon2b.cpp and icon2bc.cpp;
 * placing it in the general IconRle.h was rejected because it unnecessarily
 * changed the tracked dependency hash of unrelated icon TUs.
 *
 * Both functions crossed the shared invocation against an exact expanded
 * copy, independently, over clean plus 50 declaration-forest states:
 *
 *   build/icon2b-shared-fill-axes-20260727.json
 *   build/icon2b-shared-fill-manifest-20260727.json
 *   build/match-variants/icon2b-shared-fill-20260727/
 *   build/icon2bc-shared-fill-axes-20260727.json
 *   build/icon2bc-shared-fill-manifest-20260727.json
 *   build/match-variants/icon2bc-shared-fill-20260727/
 *
 * The macro and expanded arms were byte- and normalized-relocation-identical
 * in all 51 paired states for each target.  This establishes that the shared
 * boundary itself does not manufacture a codegen difference.
 *
 * IconToBitmap:
 *
 *   - complete matrix: 3 structures * (clean + 50 states) = 153/153;
 *   - the shared structure's matrix maximum was 81.151100%, size 1219,
 *     78/83 relocations, with the retail 80-block skeleton (62 exact and
 *     18 size-only blocks);
 *   - its low clean orbit had 79/80 blocks and is retained as a structural
 *     branch rather than discarded merely for its fuzzy score;
 *   - a canonical unchanged-source census found 27 states in 50 trials and
 *     retained an 80.142860% representative for the effective source hash.
 *
 * The matrix also kept the previous direct fill as an independent arm.  It
 * recovered the earlier 81.673080% island, so the shared form does not erase
 * that historical clue; it opens a distinct structural branch.
 *
 * IconToBitmapColorTable:
 *
 *   - complete matrix: 2 structures * (clean + 50 states) = 102/102;
 *   - every candidate preserved the exact retail 86-block topology:
 *     67 exact and 19 size-only blocks;
 *   - the retained gCTCnt publication owner was replayed independently at
 *     83.936620%, size 1449, with 91/91 relocations.
 *
 * Canonical replay artifacts:
 *
 *   build/tu-state-noise/icon2b-shared-fill-retained-20260727/
 *   build/icon2b-shared-fill-retained-summary-20260727.json
 *   build/tu-state-noise/icon2bc-shared-fill-retained-20260727/
 *   build/icon2bc-shared-fill-retained-summary-20260727.json
 *
 * The dim paths were not folded into this fragment.  Their saved destination,
 * palette transformation, loop counter, and publication ownership differ
 * materially between the two functions; parameterizing those differences
 * would create a generator rather than recover a small shared operation.
 *
 * Disposition: retain the shared forward clipped-fill fragment and both
 * per-TU invocations.  Continue from the low IconToBitmap branch as well as
 * its stronger 80-block islands; fuzzy score alone does not prune either.
 * Generated probe declarations were removed.  The match ledger was updated
 * only by the canonical scripts.
 */

void IconToBitmap_shared_forward_fill_fragment_attempt() {
}
