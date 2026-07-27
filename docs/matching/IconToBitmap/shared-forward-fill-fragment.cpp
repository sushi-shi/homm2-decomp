/*
 * IconToBitmap, RVA 0xd0570, and IconToBitmapColorTable, RVA 0xd32a0:
 * shared forward-RLE clipped-fill fragment, 2026-07-27.
 *
 * The two forward renderers have the same clipped solid-run operation after
 * substituting their per-TU row visibility, destination, X, color, count, and
 * clip owners.  This experiment represents that common source-era boundary
 * once in BASE/IconMacro.h:
 */
#if 0
#define H2_ICON_RLE_DIM_PALETTE(flags)                                                          \
    (reinterpret_cast<u8*>(uDimPal)                                                             \
     + ((flags) & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE)

#define H2_ICON_RLE_CLIPPED_FILL(                                                               \
    runVisible, row, currentX, color, runLength, clipX, clipW, clipRight)                        \
    if (runVisible) {                                                                            \
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

#define H2_ICON_RLE_FILL_RUN(                                                                    \
    clipMode, runVisible, row, currentX, color, runLength, clipX, clipW, clipRight, publishedRun) \
    if (clipMode == ICON_DRAW_NO_CLIP) {                                                          \
        memset(row + currentX, color, runLength);                                                  \
    } else                                                                                        \
        H2_ICON_RLE_CLIPPED_FILL(                                                                 \
            runVisible, row, currentX, color, runLength, clipX, clipW, clipRight                  \
        );                                                                                        \
    currentX = currentX + runLength;                                                              \
    publishedRun = runLength;                                                                     \
    continue
#endif

/*
 * Neither macro has a do/while wrapper.  H2_ICON_RLE_CLIPPED_FILL is
 * deliberately usable as the body of an `else`, preserving the original
 * `else if` CFG.  H2_ICON_RLE_FILL_RUN includes the loop's `continue`, so an
 * artificial single-iteration loop would change its meaning.  The complete
 * fill-run macro now owns the unclipped fill, clipped fill, X advance, run
 * publication, and shared tail in both forward renderers.  Both source labels
 * are named `fill_run`.
 *
 * The identical dim-palette address expression is also shared as
 * H2_ICON_RLE_DIM_PALETTE.  Moving it into the header was byte- and
 * relocation-neutral in both clean objects.  The canonical trial-1 replay
 * therefore carried the same 79.445050% and 83.737090% observations to the
 * new dependency hashes through tu_state_noise.py, without hand-editing the
 * generated ledger.
 *
 * The dedicated header is included only by Icon2b.cpp and icon2bc.cpp;
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
 *
 * The later whole-tail form was also swept independently:
 *
 *   build/tu-state-noise/icon2b-full-fill-run-macro-20260727/
 *   build/icon2b-full-fill-run-macro-states-20260727.json
 *   build/tu-state-noise/icon2bc-full-fill-run-macro-20260727/
 *   build/icon2bc-full-fill-run-macro-states-20260727.json
 *
 * IconToBitmap produced 30 states.  Trial 1 retained 79.445050%, size 1216,
 * with the exact 80-block CFG and 79/83 relocations.  The clean object is
 * 76.546700%, size 1217.  IconToBitmapColorTable produced 13 states; trial 1
 * retained 83.737090%, size 1449, the exact 86-block CFG, and 91/91
 * relocations.  Its clean object is 81.145540%, size 1457.  These MAX values
 * were recorded by tu_state_noise.py --record-max after source restoration;
 * no generated declaration remains.
 */

void IconToBitmap_shared_forward_fill_fragment_attempt() {
}
