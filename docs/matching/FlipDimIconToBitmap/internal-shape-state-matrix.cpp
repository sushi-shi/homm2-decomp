/*
FlipDimIconToBitmap (RVA 0x004daa20, retail size 571)
Internal-shape by TU-state matrix, 2026-07-27.

Artifacts:
  build/match-variants/icondf2b-small-shape-axes-20260727.json
  build/match-variants/icondf2b-small-shape-matrix-20260727.json
  build/match-variants/icondf2b-small-shape-islands-20260727/results.json

After selecting the surviving 31-block CFG family, a complete match_variants
matrix crossed 16 source shapes with the clean state and 50 requested forest
probes. All 816 variants compiled in 637.17 seconds; the source was restored
and the run was not wall-time-truncated.

Axes:
  - x0 < clipX versus clipX > x0,
  - clipX + clipW - 1 versus clipX + (clipW - 1),
  - one-expression versus staged unclipped destination construction, and
  - reuse of cn versus a dimCount snapshot in the clipped dim loop.

On the clean state, the first three axes were byte-neutral. The dimCount
snapshot produced the only second text object: 80.162650%, size 567, versus
80.192770%, size 566 for reuse_cn; both retained 24 exact plus 7 size-only
blocks.

The strongest new structural clue was trial 10 (also reproduced for reuse_cn
by trial 39): 81.698790%, size 565, all 37 relocation records present, and
26 exact plus 5 size-only blocks with no flow mismatch. It is still below the
retained 82.6747% MAX, its size is six bytes short, and its ordered relocation
stream is not exact.

The closest-size coherent islands used dimCount: trials 18 and 26 emitted size
569 with 25 exact plus 6 size-only blocks. Trial 26 scored 81.126500%; trial 18
scored 80.915665%. They are useful evidence that the remaining mismatch can
move between early scheduling and clipped-loop ownership, but neither is a
MAX improvement.

Disposition: preserve the current source. The three byte-neutral spellings are
eliminated, and dimCount remains only a lower-scoring branch clue. No generated
probe declaration or generated configuration is retained.
*/

#if 0
// Mirrored horizontal predicate; byte-neutral in every relevant clean arm.
if (clipX > x0 || w + x0 > clipW + clipX || (currentY = gFDY) < clipY
    || entry->h + currentY > clipY + clipH) {
    // existing clipped body
}

// Alternate bound materialization; byte-neutral.
gFDClipR = clipX + (clipW - 1);
gFDClipB = clipY + (clipH - 1);

// Staged unclipped destination ownership; byte-neutral.
u8* dst = gFDRow - cmd;
dst = dst + X + 1;

// Clipped dim-count snapshot; distinct but below the retained MAX.
i32 dimCount = cn;
gFDCnt2 = cn;
gFDDst = dst;
gFDCnt = 0;
if (dimCount > 0) {
    gFDCnt = dimCount;
    u32 cnt = dimCount;
    do {
        i32 px = *dst++;
        cnt--;
        gFDDst = dst;
        dst[-1] = uDimPal[0][color][px];
    } while (cnt != 0);
}
#endif
