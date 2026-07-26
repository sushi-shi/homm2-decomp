// Experiment: factor the repeated dim-pixel body into an inline helper at
// the unclipped loop, clipped loop, or both.
// Artifact: build/match-variants/iconf2bc-dim-pixel-inline-clean-20260726/
// Matrix:
//   both loops open        87.035260%, size 1341
//   helper unclipped only  73.445850%, size 1375
//   helper clipped only    71.559200%, size 1378
//   helper both            69.720406%, size 1391
// All arms retained 84/83 relocation counts.
// Disposition: rejected; unlike row visibility, this is not a retail-like
// inline boundary in either loop.

#if 0
static inline void FlipColorDimPixel(u8*& dp, u32& count)
{
    u8* dimPalette = gFCDimPal;
    i32 px = *dp;
    dp++;
    count--;
    gFCDimDst = dp;
    dp[-1] = dimPalette[px];
}

do {
    FlipColorDimPixel(dp, count);
} while (count != 0);
#endif
