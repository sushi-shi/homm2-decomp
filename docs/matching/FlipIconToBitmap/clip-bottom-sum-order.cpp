/*
FlipIconToBitmap, retail RVA 0x000d1ba0, size 0x4f1.
Vertical clip-sum operand-order matrix, 2026-07-27.

The retail setup keeps clipH live across the final vertical-bound comparison
and the gFlipClipB publication.  This could plausibly have come from the
developer writing the commutative sum in the opposite operand order.

The comparison and publication were independent axes:

    clipY + clipH  /  clipH + clipY

All four source shapes were crossed with the clean state plus 50 varied
declaration-forest states.  All 204 cells compiled in 174.40 seconds; the run
was not truncated and source was restored:

  build/iconf2b-clip-sum-order-axes-20260727.json
  build/iconf2b-clip-sum-order-manifest-20260727.json
  build/match-variants/iconf2b-clip-sum-order-20260727/results.json

MSVC 4.2 canonicalized every operand-order choice to identical bytes within
each TU state.  Clean remained 87.055700%, size 1251, 79/81 relocations, and
75/76 blocks.  Forest trial 1 reached 87.718834%, size 1249, with the same
relocation and block topology, below the retained 88.0663% MAX.

Therefore the missing setup block is not caused by the spelling order of this
commutative sum.  Preserve the simpler source.  The previously recorded
two-stage clip-bottom ownership remains the credible exact-76-block structural
island; no generated state or experimental spelling is retained.
*/

#if 0
// Representative crossed alternative.
if (clip != ICON_DRAW_NO_CLIP) {
    if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY
        || clipH + clipY < entry->h + gFlipY) {
        clip = ICON_DRAW_CLIP;
        gFlipClipR = clipX + clipW - 1;
        gFlipClipB = clipH + clipY - 1;
    } else {
        clip = ICON_DRAW_NO_CLIP;
    }
}
#endif
