// FlipIconToBitmap no-goto clipping-control structures.
//
// Matrix:
//   build/iconf2b-no-goto-clip-axes-20260727.json
// Results:
//   build/match-variants/iconf2b-no-goto-clip-clean-20260727/results.json
//
// The positive-containment spelling was byte-identical to the retained
// disjunction: 86.846150%, 1251 bytes, 79/81 relocations, 75/76 blocks.
// Staging the selection through one `outside` local produced 82.047745%,
// 1269 bytes, and 77/76 blocks. Nesting horizontal and vertical decisions
// produced 84.047745%, 1262 bytes, and 78/76 blocks. Both staged structures
// kept 79/81 relocations and introduced extra control-flow partitions.
//
// These no-goto structures were pruned before state trials because their clean
// topology adds one or two blocks beyond retail rather than supplying retail's
// one missing block in the correct structural position.

void attempted_positive_containment() {
    if (clip != ICON_DRAW_NO_CLIP) {
        if (clipX <= gFlipX0 && gFlipX0 + w <= clipX + clipW
            && clipY <= gFlipY && entry->h + gFlipY <= clipY + clipH) {
            clip = ICON_DRAW_NO_CLIP;
        } else {
            clip = ICON_DRAW_CLIP;
            gFlipClipR = clipX + clipW - 1;
            gFlipClipB = clipY + clipH - 1;
        }
    }
}

void attempted_nested_axes() {
    if (clip != ICON_DRAW_NO_CLIP) {
        i32 outside;
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w)
            outside = 1;
        else
            outside = gFlipY < clipY || clipY + clipH < entry->h + gFlipY;
        if (outside) {
            clip = ICON_DRAW_CLIP;
            gFlipClipR = clipX + clipW - 1;
            gFlipClipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
}
