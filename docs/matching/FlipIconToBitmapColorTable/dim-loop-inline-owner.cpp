// Experiment: move the complete guarded dim-pixel loop into one inline owner.
// This is distinct from the earlier per-pixel helper and loop-spelling matrices:
// the helper owns dimCount, the positive guard, gFCCnt publication, palette
// lookup, destination publication, counter update, and backedge.
//
// Artifact: build/match-variants/iconf2bc-dim-loop-owner/
// Coverage: direct/unclipped/clipped/both crossed with clean + 50 TU states
// (204/204 compiled).
// Best per structural arm:
//   direct                    87.035260%, size 1341, blocks 67 exact / 9 size
//   unclipped inline owner    74.617130%, size 1368, blocks 47 exact / 29 size
//   clipped inline owner      73.272040%, size 1376, blocks 46 exact / 30 size
//   both inline owners        72.289670%, size 1384, blocks 48 exact / 28 size
// All arms retained 84/83 relocation counts and 76/76 topology.
// Disposition: rejected structurally; whole-loop ownership creates a broad
// register-allocation orbit inconsistent with retail.

#if 0
static inline void FlipColorDimPixels(u8*& dp, u32& count) {
    i32 dimCount = count;
    gFCCnt = 0;
    if (dimCount > 0) {
        gFCCnt = dimCount;
        do {
            u8* dimPalette = gFCDimPal;
            i32 px = *dp;
            dp++;
            count--;
            gFCDimDst = dp;
            dp[-1] = dimPalette[px];
        } while (count != 0);
    }
}

gFCDimDst = dp;
FlipColorDimPixels(dp, count);
#endif
