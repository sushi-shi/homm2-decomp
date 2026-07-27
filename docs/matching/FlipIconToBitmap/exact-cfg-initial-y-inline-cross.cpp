// FlipIconToBitmap exact-CFG plus initial-Y inline-owner cross.
//
// Matrix:
//   build/iconf2b-exact-cfg-initial-inline-axes-20260727.json
// State census:
//   build/match-variants/iconf2b-exact-cfg-initial-inline-states-20260727/results.json
//
// This descendant crossed two previously independent clues: the split
// clip-bottom structure that supplies retail's 76th block, and sibling-style
// inline boundaries that read gFlipY for the side/top decision and initial row
// publication. The clean arm measured 86.116714%, 1244 bytes, and 79/81
// relocations. It aligned all 76 blocks with 66 exact bodies and ten size-only
// bodies; no target, edge-kind, or block-count mismatch remained.
//
// Clean plus 50 expansive declaration-forest states completed for both this
// arm and the retained 75-block source (102/102 builds). The exact-CFG inline
// arm peaked at 86.328910%, 1244 bytes, 79/81 relocations, still with 76/76
// aligned blocks. The retained arm reproduced the historical 88.026530% MAX.
//
// Disposition: preserve this lower island as an exact-topology branch seed.
// It does not recover either missing ordered relocation owner and did not
// close under the bounded state census, so no reconstructed source or MAX was
// changed.

static inline i32 FlipOutsideSidesOrTop(
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 width
) {
    return gFlipX0 < clipX || clipW + clipX < gFlipX0 + width
        || gFlipY < clipY;
}

static inline void FlipPublishInitialRow(bitmap* dest, i16 pitch) {
    gFlipRow = dest->m_pixels + gFlipY * pitch;
}

void attempted_exact_cfg_initial_y_cross() {
    if (clip != ICON_DRAW_NO_CLIP) {
        i32 clipBottom;
        if (FlipOutsideSidesOrTop(clipX, clipY, clipW, w)) {
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
    FlipPublishInitialRow(dest, pitch);
}
