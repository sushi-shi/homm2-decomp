/*
FlipIconToBitmap (RVA 0x000d1ba0, retail size 1265)
Clip-bottom ownership and CFG structures, 2026-07-27.

Artifacts:
  build/match-variants/iconf2b-setup-clip-axes-20260727.json
  build/match-variants/iconf2b-setup-clip-clean-islands-20260727/results.json
  build/match-variants/iconf2b-clip-cfg-descendants-20260727.json
  build/match-variants/iconf2b-clip-cfg-clean-islands-20260727/results.json

Live baseline:
  86.846150% unrounded, size 1248, 79/81 relocation records.
  It emits 75 blocks versus retail's 76. Index comparison shows only four
  exact blocks because one missing setup block shifts the entire decoder.

The initial 24-arm setup matrix crossed direct/typed entry roots, direct and
local vertical coordinates, eager clip-bottom ownership, and three clip-bound
spellings. Bound parenthesization was byte-neutral. Direct/initialized
currentY snapshots did not recover the missing block. The typed root was a
lower setup island already covered by the historical family ledger.

An eager clipBottom snapshot was the first useful lower island:
  84.644560%, size 1261, 79/81 relocations, 76/76 blocks.
  It realigned the decoder to 65 exact plus eight size-only blocks, with two
  shifted targets and one flow-kind mismatch. The extra local widened the
  frame and left an incorrect setup jump, so it was used only as a descendant.

Splitting the last vertical test from the first three failures and sharing the
bottom publication produced the strongest structure:
  85.538460%, size 1250, 79/81 relocations.
  Candidate and retail both have 76 blocks; all edges and block destinations
  are exact. There are 67 exact and nine size-only block bodies. Blocks 0, 4,
  and 6 contain setup scheduling differences; the remaining size-only blocks
  are internal dim/literal ownership differences.

This is a credible semantic branch seed despite scoring below the direct
75-block source. It does not exceed the retained 86.8594% MAX and therefore is
not retained in reconstructed source.
*/

#if 0
// Eager lower island: restores block count but widens the frame.
if (clip != ICON_DRAW_NO_CLIP) {
    i32 clipBottom = clipY + clipH;
    if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY
        || clipBottom < entry->h + gFlipY) {
        clip = ICON_DRAW_CLIP;
        gFlipClipR = clipX + clipW - 1;
        gFlipClipB = clipBottom - 1;
    } else {
        clip = ICON_DRAW_NO_CLIP;
    }
}

// Exact-CFG branch seed.
if (clip != ICON_DRAW_NO_CLIP) {
    i32 clipBottom;
    if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY) {
        clipBottom = clipY + clipH;
    } else {
        clipBottom = clipY + clipH;
        if (clipBottom >= entry->h + gFlipY) {
            clip = ICON_DRAW_NO_CLIP;
            goto clip_setup_done;
        }
    }
    clip = ICON_DRAW_CLIP;
    gFlipClipR = clipX + clipW - 1;
    gFlipClipB = clipBottom - 1;
}
clip_setup_done:
#endif
