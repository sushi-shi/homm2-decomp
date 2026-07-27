/*
FlipDimIconToBitmap (RVA 0x004daa20, retail size 571)
Whole dim-run inline ownership matrix, 2026-07-27.

Artifact:
  build/match-variants/icondf2b-row-run-owner/results.json

This matrix is distinct from the prior one-pixel helper experiment. The inline
owns the entire run: zero/count publication, destination publication, positive
guard, loop counter lifetime, source-pixel read, palette lookup, store, and
backedge. Direct, unclipped-only, clipped-only, and both-path ownership were
crossed with direct/inline row visibility and clean plus 50 forest states.
All 408 variants compiled without truncation and source was restored.

Best per run-owner family:
  direct loop, inline row      82.734940%, size 564, 31 blocks, 26 exact / 5 size
  unclipped inline owner       71.313255%, size 577, 31 blocks, 23 exact / 8 size
  clipped inline owner         71.686745%, size 585, 31 blocks, 24 exact / 7 size
  both inline owners           73.433740%, size 612, 32 blocks with 15 flow-kind
                               and 13 target-shift mismatches
All candidates retained 37/37 relocation counts.

Disposition: reject all whole-run owners. The small row predicate opens a useful
compiler orbit, but moving the already-aligned loop lifetime behind the same
kind of boundary expands code and damages block bodies/topology.
*/

#if 0
static inline void FlipDimRun(u8*& dst, u32 count, i32 color) {
    gFDCnt = 0;
    gFDDst = dst;
    if (count > 0) {
        gFDCnt = count;
        u32 cnt = count;
        do {
            i32 px = *dst++;
            cnt--;
            gFDDst = dst;
            dst[-1] = uDimPal[0][color][px];
        } while (cnt != 0);
    }
}

u8* dst = (gFDRow - cmd) + X + 1;
FlipDimRun(dst, cmd, color);

gFDCnt2 = cn;
FlipDimRun(dst, cn, color);
#endif
