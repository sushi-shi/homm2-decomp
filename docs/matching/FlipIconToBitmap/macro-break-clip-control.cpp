/*
FlipIconToBitmap, retail RVA 0x000d1ba0, size 0x4f1.
Macro-break clipping-control matrix, 2026-07-27.

The earlier exact-76-block structural island used a reconstruction goto to
share the clipped publication.  A plausible 1995 source origin is instead a
reusable multiline macro whose early success exit is represented by:

    do {
        ...
        if (fullyVisible) {
            clip = ICON_DRAW_NO_CLIP;
            break;
        }
        ... publish clipped bounds ...
    } while (0);

Four structures were crossed with the clean state plus 50 varied
declaration-forest states: the retained direct predicate, macro-break forms
with the clipBottom local inside and outside the do-block, and a natural
duplicated-publication branch.  All 204 cells compiled in 177.37 seconds, the
run was not truncated, and source was restored:

  build/iconf2b-clip-macro-control-axes-20260727.json
  build/iconf2b-clip-macro-control-manifest-20260727.json
  build/match-variants/iconf2b-clip-macro-control-20260727/results.json

Both macro-break scopes compiled identically in every state.  Their best
island was 86.190980%, size 1246, 79/81 relocations, with the exact retail CFG:
76/76 blocks, 67 exact bodies, nine size-only bodies, and no shifted targets,
flow-kind differences, or missing blocks.  This independently explains the
old goto island without requiring a literal goto in the original source.

The duplicated natural branch peaked at 83.196290%, size 1281 and 81/81 raw
relocation records.  It had 76/76 blocks with 62 exact, nine size-only, four
shifted targets and one flow-kind mismatch; ordered relocation identity still
failed.  Preserve it only as a distinct lower structural island.

The retained 75-block source reached 87.814320% in this census, below its
88.0663% historical MAX.  Neither new shape advances MAX or ordered relocation
closure.  No generated state or experimental source is retained.
*/

#if 0
// Representative exact-CFG macro expansion.
if (clip != ICON_DRAW_NO_CLIP) {
    do {
        i32 clipBottom;
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY) {
            clipBottom = clipY + clipH;
        } else {
            clipBottom = clipY + clipH;
            if (clipBottom >= entry->h + gFlipY) {
                clip = ICON_DRAW_NO_CLIP;
                break;
            }
        }
        clip = ICON_DRAW_CLIP;
        gFlipClipR = clipX + clipW - 1;
        gFlipClipB = clipBottom - 1;
    } while (0);
}
#endif
