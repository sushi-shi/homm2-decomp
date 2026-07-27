/*
 * FlipDimIconToBitmap parameter-coordinate ownership and descendants,
 * 2026-07-27.
 *
 * Retail's first setup block carries the incoming x coordinate through the
 * entry adjustments.  The live source instead introduces x0.  A complete
 * three-form matrix tested x-parameter, y-parameter, and combined x/y
 * ownership against the clean plus 50 forest states: 153/153 compiled, with
 * no truncation and source restoration.
 *
 * Only x-parameter ownership formed a credible descendant.  Forest trial 34
 * reached 82.704820%, size 564, all 37 relocations, and exact 31-block flow
 * with 26 exact plus five size-only blocks.  That improves structural
 * alignment over the live clean object's 24 exact plus seven size-only
 * blocks.  The headline trial-40 score of 83.036150% was not the better
 * structural island: it emitted size 572 with 32 candidate blocks, 13 shifted
 * and 15 flow-kind mismatches.
 *
 * The surviving x branch was then crossed with:
 *   - one-expression versus staged clipped destination construction,
 *   - cn reuse versus a dimCount snapshot, and
 *   - post-increment versus increment-then-index pixel reads.
 *
 * All 408 combinations (eight shapes times clean plus 50 forest states)
 * compiled without truncation and restored the source.  On the coherent
 * trial-34 island, destination staging and dimCount were byte-neutral; split
 * read/increment was worse.  On the malformed trial-40 island, dimCount
 * raised the fuzzy score only from 83.036150% to 83.066260%, still below the
 * retained 83.397590% MAX and without repairing topology.
 *
 * Artifacts:
 *   build/icondf2b-parameter-coordinate-ownership-axes-20260727.json
 *   build/icondf2b-parameter-coordinate-ownership-manifest-20260727.json
 *   build/match-variants/icondf2b-parameter-coordinate-ownership-20260727
 *   build/icondf2b-x-branch-internal-axes-20260727.json
 *   build/icondf2b-x-branch-internal-manifest-20260727.json
 *   build/match-variants/icondf2b-x-branch-internal-20260727
 *
 * Disposition: retain x-parameter ownership as a lower-score structural branch
 * seed, not as live source.  Neither matrix changes MAX.
 */

#if 0
// Credible coordinate-ownership branch.
gFDEntry = entry;
gFDSrc = srcData;
i32 w = entry->w;
i32 entryY = entry->y;
x = x - entry->x;
x = x - w;
x++;
gFDX0 = x;
gFDY = y + entryY;
i32 X = w + x - 1;
gFDXEnd = X;

// Tested descendants.
dst = gFDRow - cmd;
dst = dst + X + 1;

i32 dimCount = cn;
gFDCnt2 = cn;
gFDDst = dst;
gFDCnt = 0;
if (dimCount > 0) {
    gFDCnt = dimCount;
}

dst++;
i32 px = dst[-1];
#endif
