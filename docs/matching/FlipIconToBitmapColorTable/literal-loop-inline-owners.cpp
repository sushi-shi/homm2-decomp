// Experiment: move each complete mapped-literal loop into an inline owner.
// Unlike the earlier loop-form matrices, these helpers own the guard, counter
// lifetime, source advance, destination publication, lookup, and backedge.
//
// Artifact: build/match-variants/iconf2bc-literal-loop-owner-corrected/
// Coverage: direct/unclipped/clipped/both crossed with clean + 50 TU states
// (204/204 compiled).
// Best per structural arm:
//   direct                    87.035260%, size 1341, blocks 67 exact / 9 size
//   unclipped inline owner    67.055410%, size 1382, blocks 47 exact / 29 size
//   clipped inline owner      69.599495%, size 1422; only 12 exact blocks,
//                             with 23 target shifts and 28 flow mismatches
//   both inline owners        66.171290%, size 1413, blocks 48 exact / 28 size
// All arms retained 84/83 relocation counts.
//
// The first output directory, iconf2bc-literal-loop-owner/, is an invalid
// diagnostic run: a stray '+' in generated helper declarations made the three
// helper arms fail compilation. The corrected directory above is authoritative.
// Disposition: rejected structurally; these ownership boundaries are far from
// the retail block bodies and the clipped helper changes the CFG orbit.

#if 0
static inline void FlipColorLiteralUnclipped(
    u8*& src, u8*& dst, i32 count, u8* colorTable
) {
    if (count > 0) {
        gFCCnt = count;
        i32 k = count;
        do {
            i32 c = *src++;
            dst--;
            gFCDst = dst;
            dst[1] = colorTable[c];
            k--;
        } while (k != 0);
    }
}

static inline void FlipColorLiteralClipped(
    u8*& src, u32& count, i32 copyCount, u8* colorTable
) {
    if (copyCount > 0) {
        gFCCnt = copyCount;
        do {
            u8* dst = gFCDst;
            i32 c = *src++;
            dst--;
            count--;
            u8 mapped = colorTable[c];
            gFCDst = dst;
            dst[1] = mapped;
        } while (count != 0);
    }
}
#endif
