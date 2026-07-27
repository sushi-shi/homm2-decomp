/*
 * IconToBitmapColorTable, RVA 0xd32a0: dim and translated-pixel inline owners.
 *
 * The related icon blitters repeatedly expose small inline accessor
 * continuations.  This matrix tested independent single-pixel boundaries for
 * both dim loops and the literal color-table translation loop:
 */

#if 0
static inline u8* ColorTableDimPixel(u8* dst, u8* palette, i32 dimGate) {
    if (dimGate != 0)
        *dst = palette[*dst];
    return dst + 1;
}

static inline u8* ColorTableTranslatePixel(u8* dst, u8* colorTable) {
    i32 b = *gCTSrcCopy;
    dst = dst + 1;
    dst[-1] = colorTable[b];
    gCTSrcCopy = gCTSrcCopy + 1;
    return dst;
}
#endif

/*
 * Artifacts:
 *
 *   build/icon2bc-pixel-inline-combined-axes.json
 *   build/icon2bc-pixel-inline-combined-manifest.json
 *   build/match-variants/icon2bc-pixel-inline-combined/
 *
 * Complete product: direct, dim-helper, literal-helper, and combined-helper
 * shapes times clean plus 50 top-of-TU forest states, 204/204 compilations in
 * 166.08 seconds.  Both helpers inlined and source was restored.
 *
 * Best by structure:
 *
 *   dim pixel helper             82.798120%
 *   direct loops                 82.776990%
 *   both pixel helpers           82.737090%
 *   literal translation helper  82.669014%
 *
 * Every best object is size 1428 with 89/91 relocations and the same exact
 * 86-block CFG (69 exact-size, 17 size-only).  The dim helper is a credible
 * new sibling-family island and comes within 0.0141 points of the retained
 * 82.8122% MAX, but it does not exceed it.  No helper is retained.
 */

void IconToBitmapColorTable_pixel_inline_owner_attempts() {
}
